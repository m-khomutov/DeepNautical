/* 
 * File:   baseframe.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 13:27
 */

#include "baseframe.h"

baseframe::baseframe( const utils::geometry &g, int duration )
: geometry_( g )
, duration_( duration )
{}

baseframe::~baseframe()
{}

void baseframe::load( baseprotocol * proto, float duration )
{
    if( proto )
    {
        f_load( proto, duration );
    }
}

float baseframe::duration_passed( time_point_t *ts ) const
{
    time_point_t now = std::chrono::high_resolution_clock::now();
    std::chrono::duration< float, std::milli > d(now - *ts);

    bool ret = std::chrono::duration_cast< std::chrono::milliseconds >(d) >= duration_;
    if( ret )
    {
        *ts = now;
        return d.count();
    }
    return -1.f;
}
