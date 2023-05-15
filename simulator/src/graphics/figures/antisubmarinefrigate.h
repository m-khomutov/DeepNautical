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
    explicit antisubmarinefrigate( const std::vector< std::string > &settings );
    antisubmarinefrigate(const antisubmarinefrigate& orig) = delete;
    antisubmarinefrigate &operator=(const antisubmarinefrigate& orig) = delete;
    ~antisubmarinefrigate();

    void draw();

private:
    GLfloat position_[20] = {    // текстурные координаты
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f,
            0.5f, -0.5f, 0.0f,   1.0f, 1.0f,
           -0.5f, -0.5f, 0.0f,   0.0f, 1.0f,
           -0.5f,  0.5f, 0.0f,   0.0f, 0.0f,
    };
    GLuint indices_[6] = { 0, 1, 3, 1, 2, 3 };
    float waterline_ { 0.67f };

private:
    void f_check_environment() const override;
    char const *f_shader_name() const override;
    void f_initialize() override;
    void f_accept( visitor &p, double currentTime ) override;
    void f_set_model();
};

#endif /* ANTISUBMARINEFRIGATE_H */
