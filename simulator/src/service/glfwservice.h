/* 
 * File:   service.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 11:45
 */

#ifndef GLFWSERVICE_H
#define GLFWSERVICE_H

#include "baseservice.h"

class glfwservice: public baseservice {
public:
    glfwservice( basescreen *screen );

    glfwservice( glfwservice const &orig ) = delete;
    glfwservice &operator =( glfwservice const &orig ) = delete;

    ~glfwservice() = default;

    void onsignal( int ) override;
    
private:
    void f_run() override;
    int f_stop() override;
};

#endif /* SERVICE_H */
