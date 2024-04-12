/* 
 * File:   service.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 11:45
 */

#include "glfwservice.h"

glfwservice::glfwservice( TBasescreen *screen, uint16_t port )
: TBaseservice( screen, port )
{}

void glfwservice::onsignal( int )
{
    TBaseservice::stop();
}

void glfwservice::f_run()
{
}

int glfwservice::f_stop()
{
    return result_;
}
