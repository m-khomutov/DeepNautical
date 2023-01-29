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
    
    void set_vbo( GLuint vbo );
    
private:
    glm::vec3 position_[3] = {
            glm::vec3( -0.8f,  -0.8f, 0.0f ),
            glm::vec3(  0.8f,  -0.8f, 0.0f ),
            glm::vec3(  0.0f,   0.8f, 0.0f ),
    };
    glm::vec3 color_[3] = {
            glm::vec3( 1.0f, 0.0f, 0.0f ),
            glm::vec3( 0.0f, 1.0f, 0.0f ),
            glm::vec3( 0.0f, 0.0f, 1.0f ),
    };
};

#endif /* FIGURESET_H */
