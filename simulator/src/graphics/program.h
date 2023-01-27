/* 
 * File:   program.h
 * Author: mkh
 *
 * Created on 24 января 2023 г., 10:55
 */

#ifndef PROGRAM_H
#define PROGRAM_H

#include "shader.h"
#include <vector>
#include <memory>
#include <map>

class program_error: public std::runtime_error {
public:
    program_error(const std::string & what);
};

class program {
public:
    explicit program( const std::string & shader_dir );
    program(const program& orig) = delete;
    program &operator =( const program & rhs ) = delete;
    ~program();

    operator GLuint () const
    {
        return id_;
    }

    GLuint attribute_index( const std::string& name ) const;
    GLuint uniform_index( const std::string& name ) const;
    //UniformBlock * uniformBlock( const std::string& name );
    
private:
    GLuint id_;
    std::vector< std::shared_ptr< shader > > shaders_;
    std::map< std::string, std::pair< GLint/*index*/, GLint/*type*/ > > attributes_;
    std::map< std::string, std::pair< GLint/*index*/, GLint/*type*/ > > uniforms_;
    //std::map< std::string, std::shared_ptr< UniformBlock > > uniform_blocks_;

private:
    void f_check();
    void f_throw_error();
    void f_emplace_shader( const std::string & path );
    void f_link();
    void f_get_attributes();
    void f_get_uniforms();
    void f_get_uniform_blocks();
};

#endif /* PROGRAM_H */
