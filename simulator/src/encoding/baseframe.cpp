/* 
 * File:   baseframe.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 13:27
 */

#include "baseframe.h"

baseframe::baseframe()
: geometry_( utils::config()["window"] )
{
}

baseframe::~baseframe()
{
}


void baseframe::store( int width, int height )
{
    f_store( width, height );
}

void baseframe::load( baseprotocol * proto, float duration )
{
    if( proto )
    {
        f_load( proto, duration );
    }
}
