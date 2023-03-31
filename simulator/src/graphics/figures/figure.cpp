/* 
 * File:   figure.cpp
 * Author: mkh
 * 
 * Created on 31 января 2023 г., 14:35
 */

#include "figure.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

figure::figure( const std::vector< std::string > &settings )
: spec_( settings )
{
    utils::geometry win { utils::config()["window"] };
    projection_ = glm::perspective( glm::radians( 26.5f ), (GLfloat)win.width / (GLfloat)win.height, 0.1f, 100.0f );
}

figure::~figure()
{
}

void figure::initialize()
{
    program_.reset( new program( f_shader_name() ) );
    f_initialize();    
    valid_ = true;
}

void figure::draw( double currentTime )
{
    glUseProgram( *program_ );
    set_attribute( "Model", model_ );
    set_attribute( "View", view_ );
    set_attribute( "Projection", projection_ );
    if( texture_ )
    {
        texture_->activate();
    }
    f_draw( currentTime );    
}

void figure::set_attribute( const GLchar *name, float value )
{
    try {
        glProgramUniform1f ( *program_, program_->uniform_index( name ), value );
    }
    catch( const std::exception & e ) {
        GLint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glProgramUniform1f ( *program_, idx, value );
    }
}

void figure::set_attribute( const GLchar *name, GLuint value ) {
    try {
        glProgramUniform1i ( *program_, program_->uniform_index( name ), value );
    }
    catch( const std::exception & e ) {
        GLint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glProgramUniform1i ( *program_, idx, value );
    }
}

void figure::set_attribute( const GLchar *name, glm::vec3 value ) {
    try {
        glProgramUniform3f( *program_, program_->uniform_index( name ), value.x, value.y, value.z );
    }
    catch( const std::exception & e ) {
        GLint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glProgramUniform3f( *program_, idx, value.x, value.y, value.z );
    }
}

void figure::set_attribute( const GLchar *name, glm::vec4 value ) {
    try {
        glProgramUniform4f( *program_, program_->uniform_index( name ), value.x, value.y, value.z, value.w );
    }
    catch( const std::exception & e ) {
        GLint idx = glGetUniformLocation( *program_, name );
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
        GLint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glUniformMatrix3fv( idx, 1, GL_FALSE, glm::value_ptr( value ) );
    }
}

void figure::set_attribute( const GLchar *name, glm::mat4 value ) {
    try {
        glUniformMatrix4fv( program_->uniform_index( name ), 1, GL_FALSE, glm::value_ptr( value ) );
    }
    catch( const std::exception & e ) {
        GLint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glUniformMatrix4fv( idx, 1, GL_FALSE, glm::value_ptr( value ) );
    }
}

void figure::set_subroutine( const GLchar *uniform_name, const GLchar * subroutine_name, GLenum shader_type ) {
    int num;
    glGetProgramStageiv( *program_, shader_type, GL_ACTIVE_SUBROUTINES, &num );
    if( num <= 0 )
    {
        throw std::runtime_error( (shader_type == GL_VERTEX_SHADER ? std::string("vertex ") : std::string("fragment ")) +
                                  std::string("shader has no active subroutines") );
    }
    std::vector< GLuint > indices( num, 0 );
    GLint n = glGetSubroutineUniformLocation( *program_, shader_type, uniform_name );
    if( n < 0 )
    {
        throw std::runtime_error( std::string("failed to find uniform ") + uniform_name );
    }
    indices[ n ] = glGetSubroutineIndex( *program_, shader_type, subroutine_name );
    if( indices[ n ] == GL_INVALID_INDEX )
    {
        throw std::runtime_error( std::string("failed to find subroutine ") + subroutine_name );
    }
    glUniformSubroutinesuiv( shader_type, n, indices.data() );
}

void figure::set_layout( char const *name, GLuint size, GLuint step, GLuint off )
{
    try
    {
        GLuint index = program_->attribute_index( name );
        glVertexAttribPointer( index, size, GL_FLOAT, GL_FALSE, step * sizeof(GLfloat), (GLvoid*)(off * sizeof(GLfloat)) );
        glEnableVertexAttribArray( index );
    }
    catch( const std::runtime_error & e )
    {
        std::cerr << e.what() << std::endl;
    }
}
