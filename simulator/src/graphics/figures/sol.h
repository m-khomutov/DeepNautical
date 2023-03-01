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
    explicit sol( const std::vector< std::string > &settings );
    sol(sol const& orig) = delete;
    sol &operator =(sol const& orig) = delete;
    ~sol();
    
private:
    GLfloat position_[20] = {    // текстурные координаты
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f,
            0.5f, -0.5f, 0.0f,   1.0f, 1.0f,
           -0.5f, -0.5f, 0.0f,   0.0f, 1.0f,
           -0.5f,  0.5f, 0.0f,   0.0f, 0.0f,
    };
    GLuint indices_[6] = { 0, 1, 3, 1, 2, 3 };

private:
    void f_check_environment() const override;
    char const *f_shader_name() const override;
    void f_initialize() override;
    void f_draw( double currentTime ) override;
};

#endif /* SOL_H */
