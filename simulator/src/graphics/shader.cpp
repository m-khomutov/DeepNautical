/* 
 * File:   shader.cpp
 * Author: mkh
 * 
 * Created on 23 января 2023 г., 18:33
 */

#include "shader.h"

#include <fstream>
#include <vector>

shader_error::shader_error( const std::string &what )
: std::runtime_error( what )
{
}

namespace
{
    GLuint path2type( const std::string & path )
    {
        if( path.find( "/vert" ) != std::string::npos )
        {
            return GL_VERTEX_SHADER;
        }
        else if( path.find( "/frag" ) != std::string::npos )
        {
            return GL_FRAGMENT_SHADER;
        }
        throw shader_error( std::string("unknown shader type from file ") + path );
    }
}

shader::shader( const std::string & path )
: id_( glCreateShader( path2type( path ) ) )
{
    if( id_ == 0 )
        throw shader_error( std::string("Error creating shader ") + path );

    std::ifstream f(path);
    std::string s, src;
    while( !f.eof() ) {
        std::getline( f, s );
        src.append( s + "\n" );
    }
    const char * ptr = src.c_str();

    glShaderSource( id_, 1, &ptr, NULL );
    glCompileShader( id_ );
    f_verify();

    GLint cmp;
    glGetShaderiv( id_, GL_COMPILE_STATUS, &cmp );
    if( cmp == GL_FALSE )
        f_throw_error();
}

shader::~shader()
{
    glDeleteShader( id_ );
}

void shader::f_verify() {
    int glErr = glGetError();
    std::string what;
    while( glErr != GL_NO_ERROR ) {
        what.append( std::to_string( glErr ) + " ");
        glErr = glGetError();
    }
    if( !what.empty() )
    {
    	glDeleteShader( id_ );
        throw shader_error( what );
    }
}

void shader::f_throw_error() {
    int len = 0;
    glGetShaderiv( id_, GL_INFO_LOG_LENGTH, &len );
    if( len > 0 ) {
        int chWrittn = 0;
        std::vector< char > log( len, 0 );
        glGetShaderInfoLog( id_, len, &chWrittn, log.data() );
    	glDeleteShader( id_ );
        throw shader_error( log.data() );
    }
}
