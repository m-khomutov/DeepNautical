/* 
 * File:   shader.h
 * Author: mkh
 *
 * Created on 23 января 2023 г., 18:33
 */

#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <stdexcept>

class shader_error: public std::runtime_error
{
public:
    shader_error( const std::string & what );
};

class shader
{
public:
    explicit shader( const std::string & path );
    ~shader();
    shader( const shader& orig ) = delete;
    shader &operator =( const shader & rhs ) = delete;
    
    operator GLuint () const
    {
        return id_;
    }

private:
    GLuint id_;

    void f_verify();
    void f_throw_error();
};

#endif /* SHADER_H */
