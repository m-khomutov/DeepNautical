/* 
 * File:   scene.h
 * Author: mkh
 *
 * Created on 24 января 2023 г., 12:07
 */

#ifndef SCENE_H
#define SCENE_H

#include "program.h"
#include "figureset.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>


class scene_error: public std::runtime_error {
public:
    scene_error(const std::string & what);
};

class scene {
public:
    explicit scene( const std::string &shader_dir );
    scene(const scene& orig) = delete;
    scene & operator =( const scene & rhs ) = delete;
    ~scene();

    void display( GLuint width, GLuint height, double currentTime );
    
    void set_attribute( GLuint vao, const GLchar * name, float value );
    void set_attribute( GLuint vao, const GLchar * name, GLuint value );
    void set_attribute( GLuint vao, const GLchar * name, glm::vec3 );
    void set_attribute( GLuint vao, const GLchar * name, glm::vec4 );
    void set_attribute( GLuint vao, const GLchar * name, glm::mat3 );
    void set_attribute( GLuint vao, const GLchar * name, glm::mat4 );
    void set_subroutine( GLuint vao, const GLchar * uniform_name, const GLchar * subroutine_name, GLenum shader_type );

private:
    static constexpr GLuint numVAOs = 1;
    static constexpr GLuint numVBOs = 1;
    static constexpr GLuint numEBOs = 1;

    std::unique_ptr< program > program_;
    
    GLuint vao_[ numVAOs ];
    GLuint vbo_[ numVBOs ];
    GLuint ebo_[ numEBOs ];

    figureset figureset_;
    
private:
    static void GLAPIENTRY debugCb( GLenum src,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    GLsizei sz,
                                    const GLchar * msg,
                                    const void * p );

    void f_initialize( const std::string & shader_dir );
    void f_draw( double currentTime );
    void f_debug_info();
    void f_debug_error( GLenum src, GLenum type, GLuint id, GLenum severity, std::string msg ) const;
};

#endif /* SCENE_H */
