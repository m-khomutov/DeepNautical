
/* 
 * File:   base.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 14:25
 */

#include "basescreen.h"

screen_error::screen_error( const std::string &what )
: std::runtime_error( what )
{
}

basescreen::basescreen() {
}

basescreen::~basescreen() {
}

