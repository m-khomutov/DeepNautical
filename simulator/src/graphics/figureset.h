/* 
 * File:   figure_set.h
 * Author: mkh
 *
 * Created on 24 января 2023 г., 14:52
 */

#ifndef FIGURESET_H
#define FIGURESET_H

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class figureset {
public:
    figureset();
    figureset(const figureset& orig) = delete;
    figureset &operator =( const figureset &rhs ) = delete;
    ~figureset();
    
    void set_vao( GLuint vao );
    
private:
    glm::vec4 triangles_[6] = {
            glm::vec4(  0.5f,   0.5f, 0.0f, 1.0f ),
            glm::vec4(  1.0f,   0.0f, 0.0f, 1.0f ),
            glm::vec4( -0.5f,  -0.5f, 0.0f, 1.0f ),
            glm::vec4(  0.0f,   1.0f, 0.0f, 1.0f ),
            glm::vec4( -0.5f,   0.5f, 0.0f, 1.0f ),
            glm::vec4(  0.0f,   0.0f, 1.0f, 1.0f ),
    };
};

#endif /* FIGURESET_H */
