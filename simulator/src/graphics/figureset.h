/* 
 * File:   figure_set.h
 * Author: mkh
 *
 * Created on 24 января 2023 г., 14:52
 */

#ifndef FIGURESET_H
#define FIGURESET_H

#include "texture.h"

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>

class program;
class scene;

class figureset {
public:
    figureset();
    figureset(const figureset& orig) = delete;
    figureset &operator =( const figureset &rhs ) = delete;
    ~figureset();
    
    void initialize( program &prog );
    void draw( scene &sc, double currentTime );
    
private:
    GLfloat position_[20] = {    // текстурные координаты
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f,
            0.5f, -0.5f, 0.0f,   1.0f, 1.0f,
           -0.5f, -0.5f, 0.0f,   0.0f, 1.0f,
           -0.5f,  0.5f, 0.0f,   0.0f, 0.0f,
    };
    GLuint indices_[6] = { 0, 1, 3, 1, 2, 3 };
    std::unique_ptr< texture > texture_;    
    float offset_ { 0.0f };
    float offset_inc_ { -0.001f };
};

#endif /* FIGURESET_H */
