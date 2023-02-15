/* 
 * File:   water.h
 * Author: mkh
 *
 * Created on 1 февраля 2023 г., 14:18
 */

#ifndef WATER_H
#define WATER_H

#include "figure.h"

class water: public figure {
public:
    static bool environment_valid();

    water();
    water( water const &orig ) = delete;
    water &operator =( water const &orig ) = delete;
    ~water();
    
private:
    GLfloat position_[20] = {    // текстурные координаты
            1.0f, -0.17f, 0.0f,   1.0f, 0.0f,
            1.0f, -1.0f,  0.0f,   1.0f, 1.0f,
           -1.0f, -1.0f,  0.0f,   0.0f, 1.0f,
           -1.0f, -0.17f, 0.0f,   0.0f, 0.0f,
    };
    GLuint indices_[6] = { 0, 1, 3, 1, 2, 3 };

private:
    char const *f_shader_name() const override;
    void f_initialize() override;
    void f_draw( double currentTime ) override;
};

#endif /* WATER_H */

