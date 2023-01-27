/* 
 * File:   baseframe.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 13:27
 */

#include "baseframe.h"

baseframe::baseframe( int width, int height )
: width_( width )
, height_( height )
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
    f_load( proto, duration );
}
