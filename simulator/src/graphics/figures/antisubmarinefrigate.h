/* 
 * File:   antisubmarinefrigate.h
 * Author: mkh
 *
 * Created on 31 января 2023 г., 14:45
 */

#ifndef ANTISUBMARINEFRIGATE_H
#define ANTISUBMARINEFRIGATE_H

#include "figure.h"

class antisubmarinefrigate: public figure {
public:
    antisubmarinefrigate();
    antisubmarinefrigate(const antisubmarinefrigate& orig) = delete;
    antisubmarinefrigate &operator=(const antisubmarinefrigate& orig) = delete;
    ~antisubmarinefrigate();

    void initialize( program &prog ) override;
    void draw( scene &sc, double currentTime ) override;
    
private:
    GLfloat position_[20] = {    // текстурные координаты
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f,
            0.5f, -0.5f, 0.0f,   1.0f, 1.0f,
           -0.5f, -0.5f, 0.0f,   0.0f, 1.0f,
           -0.5f,  0.5f, 0.0f,   0.0f, 0.0f,
    };
    GLuint indices_[6] = { 0, 1, 3, 1, 2, 3 };
 
};

#endif /* ANTISUBMARINEFRIGATE_H */
