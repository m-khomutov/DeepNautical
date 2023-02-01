/* 
 * File:   service.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 11:45
 */

#include "service.h"

service::service()
: frame_( new jpegframe )
, screen_(new screen(frame_.get()))
, poll_( frame_.get() )
, poll_thread_( &poll_ )
{
}

service::~service()
{
}

void service::run()
{
//    screen_.reset(new screen(frame_.get()) );
    screen_->run();
}

void service::stop()
{
    screen_->stop();
//    screen_.reset(nullptr);
}
