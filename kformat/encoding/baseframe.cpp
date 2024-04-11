/* 
 * File:   baseframe.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 13:27
 */

#include "baseframe.h"

TBaseframe::TBaseframe( const NUtils::TGeometry &g, int duration )
: geometry_( g )
, duration_( duration )
{}

TBaseframe::~TBaseframe()
{}

bool TBaseframe::send_buffer( TBaseprotocol * proto )
{
    return proto ? f_send_buffer( proto ) : false;
}

float TBaseframe::is_duration_passed( time_point_t *ts ) const
{
    time_point_t now = std::chrono::high_resolution_clock::now();
    std::chrono::duration< float, std::milli > delta(now - *ts);

    if( std::chrono::duration_cast< std::chrono::milliseconds >(delta) >= duration_ )  // время кадра истекло
    {
        *ts = now;
        return delta.count();
    }
    return -1.f;
}
