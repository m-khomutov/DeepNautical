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

struct debug_message {
    GLenum source;
    GLenum type;
    GLuint id;
    GLenum severity;
    std::string body;

    debug_message &set_source( GLenum src )
    {
        source = src;
        return *this;
    };
    debug_message &set_type( GLenum t )
    {
        type = t;
        return *this;
    };
    debug_message &set_id( GLenum i )
    {
         id = i;
         return *this;
    };
    debug_message &set_severity( GLenum s )
    {
        severity = s;
        return *this;
    };
    debug_message &set_body( const GLchar * msg, GLsizei sz )
    {
        body = std::string( msg, sz );
        return *this;
    }
};


class scene {
public:
    explicit scene( const std::string &specification );
    scene(const scene& orig) = delete;
    scene & operator =( const scene & rhs ) = delete;
    ~scene();

    size_t cameras() const
    {
        return figureset_.size();
    }

    void display( size_t view, GLuint width, GLuint height, double currentTime );
    
private:
    std::vector< std::shared_ptr< figureset > > figureset_;
    
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
    void f_debug_error( const debug_message &msg ) const;
    void f_add_figure( const std::string &header, const std::vector< std::string > &settings, const glm::vec3 &camera_pos );

    template< typename Figure >
    void f_add_figure( const std::vector< std::string > &settings, const glm::vec3 &camera_pos );

    std::vector< glm::vec3 > f_get_camera_positions( const std::vector< std::string > &settings );
};

#endif /* SCENE_H */
