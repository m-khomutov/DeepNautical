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
    explicit sol( const std::vector< std::string > &settings, const glm::vec3 &camera_pos );
    sol(sol const& orig) = delete;
    sol &operator =(sol const& orig) = delete;
    ~sol();
    
    void draw( size_t vbo_number );

private:
    GLfloat position_[20] = {    // текстурные координаты
           -2.0f,  1.0f, -1.0f,   1.0f, 0.0f,
            2.5f,  1.0f, -1.0f,   1.0f, 1.0f,
            2.5f,  0.0f, -1.0f,   0.0f, 1.0f,
           -2.0f,  0.0f, -1.0f,   0.0f, 0.0f,
    };
    GLuint indices_[6] = { 0, 3, 1, 1, 3, 2 };

private:
    void f_check_environment() const override;
    char const *f_shader_name() const override;
    void f_initialize( size_t vbo_number ) override;
    void f_accept( size_t vbo_number, visitor &p, double currentTime ) override;
};

#endif /* SOL_H */
