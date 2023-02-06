/* 
 * File:   viewer.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 17:49
 */

#include "gviewer.h"
#include <gtk/gtk.h>
#include <iostream>

namespace
{
void destroy_viewer( GtkWidget *widget, gpointer data )
{
    if( data )
    {
        reinterpret_cast< g_viewer *>(data)->stop();
    }
}
gboolean update_viewer( gpointer data )
{
    reinterpret_cast< g_viewer *>(data)->update();
    return true;
}

GdkPixbuf *create_pixbuf(const gchar * filename)
{
   GError *error = NULL;
   GdkPixbuf *rc = gdk_pixbuf_new_from_file( filename, &error );
   if( !rc )
   {
      std::cerr << error->message << std::endl;
      g_error_free( error );
   }
   return rc;
}

}  // namespace

g_viewer::g_viewer( int argc, char *argv[] )
{
    gtk_init( &argc, &argv );
    
    window_ = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    layout_ = gtk_box_new( GTK_ORIENTATION_VERTICAL, 0 );
    allocation_.reset( g_new(GtkAllocation, 1) );

    utils::geometry window = utils::config()["window"];
    gtk_window_set_title( GTK_WINDOW(window_), "ПО управления" );
    gtk_window_set_default_size( GTK_WINDOW(window_), window.width, window.height );
    gtk_window_set_position( GTK_WINDOW(window_), GTK_WIN_POS_CENTER );

    //GdkPixbuf *icon = create_pixbuf( "/home/mkh/Изображения/awesomeface.png" );  
    //gtk_window_set_icon( GTK_WINDOW(window_), icon );
    
    gtk_container_add( GTK_CONTAINER(window_), layout_ );
    gtk_widget_show( window_ );
    
    g_signal_connect( window_, "destroy", G_CALLBACK(destroy_viewer), this );
    
    //g_object_unref( icon );
}


void g_viewer::onsignal( int )
{
    baseviewer::stop();
}

void g_viewer::f_run()
{
    update_tag_ = g_timeout_add( 40,  update_viewer, this );
    gtk_main();
}

int g_viewer::f_stop()
{
    if( update_tag_ != -1 )
    {
        gtk_main_quit();
        update_tag_ = -1;
    }
    return EXIT_SUCCESS; 
}

void g_viewer::update()
{
    if( decoder_->load( &frame_ ) )
    {

        if( image_ )
        {
            gtk_widget_destroy( GTK_WIDGET(image_) );
        }
        GdkPixbuf* pb = gdk_pixbuf_new_from_data( frame_.pixels.data(),
                                                  GDK_COLORSPACE_RGB,
                                                  FALSE,//has_alpha - 4-th byte
                                                  8,
                                                  frame_.window.width,
                                                  frame_.window.height,
                                                  frame_.window.width * frame_.channels,
                                                  nullptr, nullptr );

        gtk_widget_get_allocation( window_, allocation_.get() );
        if( allocation_->width != frame_.window.width || allocation_->height != frame_.window.height )
        {
            GdkPixbuf *pbs = gdk_pixbuf_scale_simple( pb,
                                                      allocation_->width,
                                                      allocation_->height,
                                                      GDK_INTERP_BILINEAR );
            image_ = gtk_image_new_from_pixbuf( pbs );
            g_object_unref( pbs );
        }
        else
        {
            image_ = gtk_image_new_from_pixbuf( pb );
        }
        g_object_unref( pb );

        gtk_box_pack_start( GTK_BOX (layout_), image_, FALSE, FALSE, 0 );
        gtk_widget_show( image_ );
        gtk_widget_show( layout_ );
    }
}
