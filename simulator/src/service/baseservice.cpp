/* 
 * File:   base.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 16:14
 */

#include "baseservice.h"

baseservice::baseservice( basescreen *screen )
: poll_( screen )
, poll_thread_( &poll_ )
{}

baseservice::~baseservice()
{}

void baseservice::run()
{
    f_run();
}

int baseservice::stop()
{
    return f_stop();
}
