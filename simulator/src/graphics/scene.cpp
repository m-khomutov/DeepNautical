/* 
 * File:   scene.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 12:07
 */

#include "scene.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace {
    std::string severity2str( GLuint severity ) {
        switch( severity ) {
            case GL_DEBUG_SEVERITY_HIGH:
                return "Ошибка или опасная ситуация";
            case GL_DEBUG_SEVERITY_MEDIUM:
                return "Вожное предупреждение";
            case GL_DEBUG_SEVERITY_LOW:
                return "Предупреждение";
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                return "Извещение";
        }
        return std::to_string( severity );
    }
}


scene_error::scene_error( const std::string &what )
: std::runtime_error( what )
{
}

void GLAPIENTRY
scene::debugCb( GLenum src,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei sz,
                const GLchar * msg,
                const void * p )
{
    reinterpret_cast< const scene * >(p)->f_debug_error( src, type, id, severity, std::string(msg, sz) );
}

scene::scene()
{
//    glEnable( GL_DEBUG_OUTPUT );
    glEnable( GL_DEPTH_TEST );
//    glDebugMessageCallback( scene::debugCb, this );
//    glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    f_debug_info();
    f_initialize();
}

scene::~scene()
{
}

void scene::display( GLuint width, GLuint height, double currentTime )
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //m_delta = std::isinf( m_delta ) ? currentTime : currentTime - m_delta;
    glUseProgram( *program_ );
    glBindVertexArray( vao_[0] );
    f_draw( currentTime );
    glBindVertexArray( 0 );
}

void scene::set_attribute( const GLchar *name, float value )
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

void scene::set_attribute( const GLchar *name, GLuint value ) {
    try {
        glProgramUniform1i ( *program_, program_->uniform_index( name ), value );
    }
    catch( const std::exception & e ) {
        GLuint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glProgramUniform1i ( *program_, idx, value );
    }
}

void scene::set_attribute( const GLchar *name, glm::vec3 value ) {
    try {
        glProgramUniform3f( *program_, program_->uniform_index( name ), value.x, value.y, value.z );
    }
    catch( const std::exception & e ) {
        GLuint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glProgramUniform3f( *program_, idx, value.x, value.y, value.z );
    }
}

void scene::set_attribute( const GLchar *name, glm::vec4 value ) {
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

void scene::set_attribute( const GLchar *name, glm::mat3 value ) {
    try {
        glUniformMatrix3fv( program_->uniform_index( name ), 1, GL_FALSE, glm::value_ptr( value ) );
    }
    catch( const std::exception & e ) {
        GLuint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glUniformMatrix3fv( idx, 1, GL_FALSE, glm::value_ptr( value ) );
    }
}

void scene::set_attribute( const GLchar *name, glm::mat4 value ) {
    try {
        glUniformMatrix4fv( program_->uniform_index( name ), 1, GL_FALSE, glm::value_ptr( value ) );
    }
    catch( const std::exception & e ) {
        GLuint idx = glGetUniformLocation( *program_, name );
        if( idx != -1 )
            glUniformMatrix4fv( idx, 1, GL_FALSE, glm::value_ptr( value ) );
    }
}

void scene::set_subroutine( const GLchar *uniform_name, const GLchar * subroutine_name, GLenum shader_type ) {
    int num;
    glGetProgramStageiv( *program_, shader_type, GL_ACTIVE_SUBROUTINES, &num );
    if( num > 0 ) {
        std::vector< GLuint > indices( num, 0 );
        GLuint n = glGetSubroutineUniformLocation( *program_, shader_type, uniform_name );
        indices[ n ] = glGetSubroutineIndex( *program_, shader_type, subroutine_name );
        glUniformSubroutinesuiv( shader_type, 1, indices.data() );
    }
}

void scene::f_initialize()
{
    program_.reset( new program );
    
    glGenVertexArrays( numVAOs, vao_ );
    glBindVertexArray( vao_[0] );
    glGenBuffers( numEBOs, ebo_ );
    glGenBuffers( numVBOs, vbo_ );
    glBindBuffer( GL_ARRAY_BUFFER, vbo_[0] );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo_[0] );
    figureset_.initialize( *program_ );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
}

void scene::f_draw( double currentTime )
{
    figureset_.draw( *this, currentTime );
}

void scene::f_debug_info()
{
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cerr << "OpenGL: {\n\tvendor: " << glGetString( GL_VENDOR )
              << "\n\trenderer: " << glGetString( GL_RENDERER )
              << "\n\tversion: " << glGetString( GL_VERSION ) << " (" << major << "." << minor << ")"
              << "\n\tGLSL version: " << glGetString( GL_SHADING_LANGUAGE_VERSION )
              << "\n\textensions: [ ";
    GLint nExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);
    for( int i = 0; i < nExtensions; i++ )
        std::cerr<< glGetStringi( GL_EXTENSIONS, i ) << " ";
    std::cerr << "]\n}" << std::endl;   
}

void scene::f_debug_error( GLenum src,
                           GLenum type,
                           GLuint id,
                           GLenum severity,
                           std::string msg ) const
{
    std::cerr << src << ":" << type <<"[" << severity2str(severity) <<"](" <<id <<") :" << msg << std::endl;
}
