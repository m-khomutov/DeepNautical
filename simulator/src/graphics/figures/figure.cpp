/* 
 * File:   figure.cpp
 * Author: mkh
 * 
 * Created on 31 января 2023 г., 14:35
 */

#include "figure.h"
#include <glm/gtc/type_ptr.hpp>

figure::figure()
{
}

figure::~figure()
{
}

void figure::set_attribute( const GLchar *name, float value )
{
    try {
        glProgramUniform1f ( *program_, program_->uniform_index( name ), value );
    }
    catch( const std::exception & e ) {
        GLuint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glProgramUniform1f ( *program_, idx, value );
    }
}

void figure::set_attribute( const GLchar *name, GLuint value ) {
    try {
        glProgramUniform1i ( *program_, program_->uniform_index( name ), value );
    }
    catch( const std::exception & e ) {
        GLuint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glProgramUniform1i ( *program_, idx, value );
    }
}

void figure::set_attribute( const GLchar *name, glm::vec3 value ) {
    try {
        glProgramUniform3f( *program_, program_->uniform_index( name ), value.x, value.y, value.z );
    }
    catch( const std::exception & e ) {
        GLuint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glProgramUniform3f( *program_, idx, value.x, value.y, value.z );
    }
}

void figure::set_attribute( const GLchar *name, glm::vec4 value ) {
    try {
        glProgramUniform4f( *program_, program_->uniform_index( name ), value.x, value.y, value.z, value.w );
    }
    catch( const std::exception & e ) {
        GLuint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 ) {
            glProgramUniform4f( *program_, idx, value.x, value.y, value.z, value.w );
        }
    }
}

void figure::set_attribute( const GLchar *name, glm::mat3 value ) {
    try {
        glUniformMatrix3fv( program_->uniform_index( name ), 1, GL_FALSE, glm::value_ptr( value ) );
    }
    catch( const std::exception & e ) {
        GLuint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glUniformMatrix3fv( idx, 1, GL_FALSE, glm::value_ptr( value ) );
    }
}

void figure::set_attribute( const GLchar *name, glm::mat4 value ) {
    try {
        glUniformMatrix4fv( program_->uniform_index( name ), 1, GL_FALSE, glm::value_ptr( value ) );
    }
    catch( const std::exception & e ) {
        GLuint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glUniformMatrix4fv( idx, 1, GL_FALSE, glm::value_ptr( value ) );
    }
}

void figure::set_subroutine( const GLchar *uniform_name, const GLchar * subroutine_name, GLenum shader_type ) {
    int num;
    glGetProgramStageiv( *program_, shader_type, GL_ACTIVE_SUBROUTINES, &num );
    if( num > 0 ) {
        std::vector< GLuint > indices( num, 0 );
        GLuint n = glGetSubroutineUniformLocation( *program_, shader_type, uniform_name );
        indices[ n ] = glGetSubroutineIndex( *program_, shader_type, subroutine_name );
        glUniformSubroutinesuiv( shader_type, 1, indices.data() );
    }
}
