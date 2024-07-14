/* 
 * File:   baseframe.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 13:27
 */

#include "baseframe.h"

frame::base::base( const utils::winsize &sz, int duration )
: size_( sz )
, duration_( duration )
{}

frame::base::~base()
{}

bool frame::base::send_buffer( proto::base * pr )
{
    return pr ? f_send_buffer( pr ) : false;
}

float frame::base::is_duration_passed( time_point_t *ts ) const
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
