/* 
 * File:   figure.cpp
 * Author: mkh
 * 
 * Created on 31 января 2023 г., 14:35
 */

#include "figure.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <fstream>
#include <iterator>
#include <sstream>

namespace
{
    template< typename T >
    bool str2vec( const std::string &s, T *rc )
    {
        std::istringstream iss(s);
        std::vector< std::string > v( (std::istream_iterator<std::string>(iss)),
                                       std::istream_iterator<std::string>() );
        if( v.size() == sizeof(T) / sizeof(GLfloat) + 1 )
        {
            for( GLuint i(1); i < v.size(); ++i )
            { 
                (*rc)[i - 1] = std::stod(v[i]);
            }
            return true;
        }
        return false;
    }
    
    bool str2face( const std::string &s, objreader::face_t *rc )
    {
        return sscanf( s.c_str(), "f %u/%u/%u %u/%u/%u %u/%u/%u",
                       &((*rc)[0][0]), &((*rc)[0][1]), &((*rc)[0][2]),
                       &((*rc)[1][0]), &((*rc)[1][1]), &((*rc)[1][2]),
                       &((*rc)[2][0]), &((*rc)[2][1]), &((*rc)[2][2]) ) == 9;
    }
}  //namespace


objreader::objreader( const char* filename )
{
    std::ifstream ifile( filename );
    if( !ifile.is_open() )
    {
        throw std::runtime_error( std::string(filename) +" error: " +strerror(errno) );
    }
    
    glm::vec3 v3;
    glm::vec2 v2;
    std::string line;
    while( std::getline( ifile, line ) )
    {
        if( line[ 0 ] == 'v' )
        {
            switch( line[1] )
            {
            case ' ':
                if( str2vec( line, &v3 ) )
                {
                    vertices_.push_back( v3 );
                }
                break;
            case 't':
                if( str2vec( line, &v2 ) )
                {
                    textures_.push_back( v2 );
                }
                break;
            case 'n':
                if( str2vec( line, &v3 ) )
                {
                    normals_.push_back( v3 );
                }
                break;
            }
        }
        else if( line[ 0 ] == 'f' )
        {
            face_t face;
            if( str2face( line, &face ) )
            {
                faces_.push_back( face );
            }
        }
    }
}

void objreader::load_position( std::vector< GLfloat > *pos )
{
    for( auto f: faces_ )
    {
        for( int i(0); i < 3; ++i )
        {
            pos->push_back( vertices_[f[i].x - 1].x );
            pos->push_back( vertices_[f[i].x - 1].y );
            pos->push_back( vertices_[f[i].x - 1].z );
            pos->push_back( textures_[f[i].y - 1].x );
            pos->push_back( textures_[f[i].y - 1].y );
            pos->push_back( normals_[f[i].z - 1].x );
            pos->push_back( normals_[f[i].z - 1].y );
            pos->push_back( normals_[f[i].z - 1].z );
        }
    }
}


figure::figure()
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
    if( num > 0 ) {
        std::vector< GLuint > indices( num, 0 );
        GLuint n = glGetSubroutineUniformLocation( *program_, shader_type, uniform_name );
        indices[ n ] = glGetSubroutineIndex( *program_, shader_type, subroutine_name );
        glUniformSubroutinesuiv( shader_type, 1, indices.data() );
    }
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
