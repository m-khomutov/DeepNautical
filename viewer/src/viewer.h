/* 
 * File:   viewer.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 17:49
 */

#ifndef VIEWER_H
#define VIEWER_H

#include "receiver/receiver.h"
#include "decoder/basedecoder.h"
#include "../../share/utils.h"
#include <gtk/gtk.h>
#include <stdexcept>

class basedecoder;

class viewer_error: public std::runtime_error
{
public:
    viewer_error( const std::string & what );
};

class viewer {
public:
    viewer();
    viewer(const viewer& orig) = delete;
    viewer &operator =(const viewer& orig) = delete;
    ~viewer();

    void run();
    void stop();
    void update();

private:
    struct del { void operator ()(GtkAllocation *p) { g_free(p); } };
    
    decframe frame_;
    std::unique_ptr< basedecoder > decoder_;
    receiver receiver_;
    utils::scoped_thread< receiver > rec_thread_;
    gint update_tag_ { 0 }; 

    GtkWidget *window_;
    GtkWidget *layout_; 
    GtkWidget *image_ = nullptr;
    std::unique_ptr< GtkAllocation, del > allocation_;
};

#endif /* VIEWER_H */
