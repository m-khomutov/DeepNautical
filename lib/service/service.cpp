/* 
 * File:   base.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 16:14
 */

#include "service.h"
#include "screen.h"

base::service::service( base::screen *screen, uint16_t port )
    : screen_( screen )
    , loop_( screen, port )
    , loop_thread_( &loop_ )
{}

// поток работы с сетью автоматом не стартует. Надо запускать функцией start_vdev_capture
base::service::service( char const *videodevname, uint16_t port )
    : loop_( videodevname, port )
{}

void base::service::start_screen()
{
    std::string what = loop_thread_.exception();
    if( !what.empty() )
    {
        throw std::runtime_error( what );
    }

    if( screen_ )
    {
        screen_->run();
        f_start_screen();
    }
}

int base::service::stop_screen()
{
    if( screen_ )
    {
        screen_->stop();
    }
    return f_stop_screen();
}

void base::service::start_vdev_capture()
{
    loop_.start();
}

void base::service::stop_vdev_capture()
{
    loop_.stop();
}
