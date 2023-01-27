/* 
 * File:   service.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 11:45
 */

#include "service.h"

service::service( uint16_t b_port, int width, int height, int quality, int duration )
: frame_( new jpegframe( width, height, quality ) )
, screen_( frame_.get() )
, poll_( b_port, duration, frame_.get() )
, poll_thread_( &poll_ )
{
}

service::~service()
{
}

void service::run( const char *shader_dir )
{
    screen_.run( shader_dir );
}

void service::stop()
{
    screen_.stop();
}
