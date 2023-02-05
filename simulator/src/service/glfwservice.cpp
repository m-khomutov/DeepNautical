/* 
 * File:   service.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 11:45
 */

#include "glfwservice.h"
#include "../graphics/screens/glfwscreen.h"

glfwservice::glfwservice()
{
     screen_.reset( new glfwscreen( frame_.get() ) );
}

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
