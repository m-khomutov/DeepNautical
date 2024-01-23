/* 
 * File:   service.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 11:45
 */

#include "glfwservice.h"

glfwservice::glfwservice( basescreen *screen, uint16_t port )
: baseservice( screen, port )
{}

void glfwservice::onsignal( int )
{
    baseservice::stop();
}

void glfwservice::f_run()
{
    ;
}

int glfwservice::f_stop()
{
    return result_;
}
