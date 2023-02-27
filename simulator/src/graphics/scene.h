/* 
 * File:   scene.h
 * Author: mkh
 *
 * Created on 24 января 2023 г., 12:07
 */

#ifndef SCENE_H
#define SCENE_H

#include "figureset.h"
#include <GL/glew.h>
#include <string>


class scene_error: public std::runtime_error {
public:
    scene_error(const std::string & what);
};

class scene {
public:
    explicit scene( const std::string &specification );
    scene(const scene& orig) = delete;
    scene & operator =( const scene & rhs ) = delete;
    ~scene();

    void display( GLuint width, GLuint height, double currentTime );
    
private:
    figureset figureset_;
    
private:
    static void GLAPIENTRY debugCb( GLenum src,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    GLsizei sz,
                                    const GLchar * msg,
                                    const void * p );

    void f_initialize( const std::string &specification );
    void f_debug_info();
    void f_debug_error( GLenum src, GLenum type, GLuint id, GLenum severity, std::string msg ) const;
    void f_add_figure( const std::string &header, const std::vector< std::string > &settings );

    template< typename Figure >
    void f_add_figure( const std::vector< std::string > &settings );
};

#endif /* SCENE_H */
