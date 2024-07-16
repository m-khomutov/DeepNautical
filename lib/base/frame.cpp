/* 
 * File:   baseframe.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 13:27
 */

#include "frame.h"

base::frame::frame( const utils::winsize &sz, int duration )
    : size_( sz )
    , duration_( duration )
{}

base::frame::~frame()
{}

bool base::frame::send_buffer( base::protocol * pr )
{
    return pr ? f_send_buffer( pr ) : false;
}

float base::frame::expired( time_point_t *ts ) const
{
    time_point_t now = std::chrono::high_resolution_clock::now();
    std::chrono::duration< float, std::milli > delta(now - *ts);

    if( std::chrono::duration_cast< std::chrono::milliseconds >(delta) >= duration_ )
    {
        *ts = now;
        return delta.count();
    }
    return -1.f;
}
