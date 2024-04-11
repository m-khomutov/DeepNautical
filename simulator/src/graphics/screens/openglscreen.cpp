
/* 
 * File:   openglscreen.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 14:25
 */

#include "openglscreen.h"

screen_error::screen_error( const std::string &what )
: std::runtime_error( what )
{
}

openglscreen::openglscreen( TBaseframe *frame )
: basescreen( frame )
{
    NUtils::read_directory( NUtils::config()["scenes"],
                           ".scn",
                           [this]( const std::string &name )
                           {
                               scenes_.insert( name.substr( 0, name.size() - 4 ) );
                           } );
    if( scenes_.empty() )
        throw screen_error( std::string("no scenes found") );
    
    scene_iter_ = scenes_.begin();
}

openglscreen::~openglscreen()
{}

void openglscreen::set_scene( const std::string &scene )
{
    if( scene == *scene_iter_ )
    {
        return;
    }
    std::set< std::string>::iterator it = scenes_.find( scene );
    if( it == scenes_.end() )
    {
        throw screen_error( scene + " not found" );
    }
    scene_iter_ = it;

    auto g = commands_.get();
    g->emplace_back( command::set_scene );
}

void openglscreen::f_exec_command()
{
    auto g = commands_.get();
    if( ! g->empty() )
    {
        command cmd = g->front();
        g->pop_front();
        switch( cmd.type() )
        {
        case command::set_scene:
            sc_.back().reset(new scene( std::string(NUtils::config()["scenes"]) + "/" + *scene_iter_ + ".scn" ) );
            break;
        }
    }
}

void openglscreen::f_store()
{
    int q;
    glGetIntegerv( GL_READ_BUFFER, &q );
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    //glReadBuffer( GL_FRONT);//COLOR_ATTACHMENT0 );
    for( size_t v(0); v < viewes_; ++v )
    {
        uint8_t *buffer = frame_->buffer( v, frame_->width(), frame_->height() );
        glReadPixels( v * frame_->width(), 0, frame_->width(), frame_->height(), GL_RGB, GL_UNSIGNED_BYTE, buffer );
    }
}

bool openglscreen::f_load( TBaseprotocol *proto )
{
   return frame_->send_buffer( proto );
}
