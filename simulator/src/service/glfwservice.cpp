/* 
 * File:   service.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 11:45
 */

#include "glfwservice.h"

glfwservice::glfwservice( basescreen *screen, uint16_t port, uint32_t duration )
: baseservice( screen, port, duration )
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
