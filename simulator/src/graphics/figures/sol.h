/* 
 * File:   sol.h
 * Author: mkh
 *
 * Created on 1 февраля 2023 г., 10:12
 */

#ifndef SOL_H
#define SOL_H

#include "figure.h"

class sol: public figure {
public:
    sol();
    sol(sol const& orig) = delete;
    sol &operator =(sol const& orig) = delete;
    ~sol();
    
    void initialize() override;
    void draw( double currentTime ) override;

private:
    GLfloat position_[20] = {    // текстурные координаты
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f,
            0.5f, -0.5f, 0.0f,   1.0f, 1.0f,
           -0.5f, -0.5f, 0.0f,   0.0f, 1.0f,
           -0.5f,  0.5f, 0.0f,   0.0f, 0.0f,
    };
    GLuint indices_[6] = { 0, 1, 3, 1, 2, 3 };
};

#endif /* SOL_H */
