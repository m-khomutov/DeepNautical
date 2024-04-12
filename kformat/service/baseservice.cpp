/* 
 * File:   base.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 16:14
 */

#include "baseservice.h"
#include "screen/basescreen.h"

baseservice::baseservice( TBasescreen *screen, uint16_t port )
: screen_( screen )
, poll_( screen, port )
, poll_thread_( &poll_ )
{}

baseservice::baseservice( char const *videodevname, uint16_t port )
: poll_( videodevname, port )
{}

baseservice::~baseservice()
{}

void baseservice::run()
{
    if( screen_ )
    {
        screen_->run_scene_display();
    }
    else
    {
        poll_.run();
    }
    f_run();
}

int baseservice::stop()
{
    if( screen_ )
    {
        screen_->stop_scene_display();
    }
    else
    {
        poll_.stop();
    }
    return f_stop();
}
