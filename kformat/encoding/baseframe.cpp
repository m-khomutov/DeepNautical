/* 
 * File:   baseframe.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 13:27
 */

#include "baseframe.h"

baseframe::baseframe( const utils::geometry &g )
: geometry_( g )
{}

baseframe::~baseframe()
{}


void baseframe::store()
{
    f_store();
}

void baseframe::load( baseprotocol * proto, float duration )
{
    if( proto )
    {
        f_load( proto, duration );
    }
}
