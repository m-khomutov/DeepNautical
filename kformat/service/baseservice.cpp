/* 
 * File:   base.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 16:14
 */

#include "baseservice.h"
#include "screen/basescreen.h"

baseservice::baseservice( basescreen *screen, uint16_t port, uint32_t duration )
: screen_( screen )
, poll_( screen, port, duration )
, poll_thread_( &poll_ )
{}

baseservice::~baseservice()
{}

void baseservice::run()
{
    screen_->run();
    f_run();
}

int baseservice::stop()
{
    screen_->stop();
    screen_.reset();

    return f_stop();
}
