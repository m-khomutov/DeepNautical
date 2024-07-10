/* 
 * File:   uniformblock.cpp
 * Author: mkh
 * 
 * Created on 29 января 2023 г., 11:52
 */

#include "uniformblock.h"
#include <cstring>
#include <iostream>


uniformblock_error::uniformblock_error( const std::string &what )
: std::runtime_error( what )
{
}


namespace
{
size_t typesz( GLenum type )
{
    switch( type )
    {
    case GL_FLOAT:             return 1 * sizeof(GLfloat);
    case GL_FLOAT_VEC2:        return 2 * sizeof(GLfloat);
    case GL_FLOAT_VEC3:        return 3 * sizeof(GLfloat);
    case GL_FLOAT_VEC4:        return 4 * sizeof(GLfloat);
    case GL_INT:               return 1 * sizeof(GLint);
    case GL_INT_VEC2:          return 2 * sizeof(GLint);
    case GL_INT_VEC3:          return 3 * sizeof(GLint);
    case GL_INT_VEC4:          return 4 * sizeof(GLint);
    case GL_UNSIGNED_INT:      return 1 * sizeof(GLuint);
    case GL_UNSIGNED_INT_VEC2: return 2 * sizeof(GLuint);
    case GL_UNSIGNED_INT_VEC3: return 3 * sizeof(GLuint);
    case GL_UNSIGNED_INT_VEC4: return 4 * sizeof(GLuint);
    case GL_BOOL:              return 1 * sizeof(GLboolean);
    case GL_BOOL_VEC2:         return 2 * sizeof(GLboolean);
    case GL_BOOL_VEC3:         return 3 * sizeof(GLboolean);
    case GL_BOOL_VEC4:         return 4 * sizeof(GLboolean);
    case GL_FLOAT_MAT2:        return 4 * sizeof(GLfloat);
    case GL_FLOAT_MAT2x3:      return 6 * sizeof(GLfloat);
    case GL_FLOAT_MAT2x4:      return 8 * sizeof(GLfloat);
    case GL_FLOAT_MAT3:        return 9 * sizeof(GLfloat);
    case GL_FLOAT_MAT3x2:      return 6 * sizeof(GLfloat);
    case GL_FLOAT_MAT3x4:      return 12 * sizeof(GLfloat);
    case GL_FLOAT_MAT4:	       return 16 * sizeof(GLfloat);
    case GL_FLOAT_MAT4x2:      return 8 * sizeof(GLfloat);
    case GL_FLOAT_MAT4x3:      return 12 * sizeof(GLfloat);
    }
    throw uniformblock_error( std::string("invalid type: ") + std::to_string(type) );
}
}  // namespace

uniformblock::variable::variable( const GLchar *name, GLint index, GLenum type )
: name_( name )
, index_( index )
, type_( type )
, value_( typesz( type ) )
{
}


uniformblock::uniformblock( GLuint program_id, const GLchar *name )
: program_id_( program_id )
, id_( glGetUniformBlockIndex( program_id, name ) )
, name_( name )
{
    if( id_ == -1 )
        throw uniformblock_error( name + std::string(" block not found") );
    
    GLint sz;
    glGetActiveUniformBlockiv( program_id, id_, GL_UNIFORM_BLOCK_DATA_SIZE, &sz );
    buffer_.resize( sz );
    glGenBuffers( 1, &ubo_ );
}


uniformblock::~uniformblock() {
}

void uniformblock::emplace_variable( std::string const &name, GLint index, GLenum type )
{
    variables_.emplace_back( name.c_str(), index, type );
}

void uniformblock::copy()
{
    glBindBuffer( GL_UNIFORM_BUFFER, ubo_ );
    
    const GLchar *names[variables_.size()];
    for( std::vector< variable >::size_type i(0); i < variables_.size(); ++i )
        names[i] = variables_[i].name().c_str();
    GLuint indices[variables_.size()];
    glGetUniformIndices( program_id_, variables_.size(), names, indices );
    GLint offset[variables_.size()];
    glGetActiveUniformsiv( program_id_, variables_.size(), indices, GL_UNIFORM_OFFSET, offset );
    for( std::vector< variable >::size_type i(0); i < variables_.size(); ++i )
        ::memcpy( buffer_.data() + offset[i], variables_[i], variables_[i].size() );
    glBufferData( GL_UNIFORM_BUFFER, buffer_.size(), buffer_.data(), GL_DYNAMIC_DRAW );
    glBindBufferBase( GL_UNIFORM_BUFFER, id_, ubo_ );
}

uniformblock::variable &uniformblock::operator []( GLchar const *name )
{
    for( std::vector< variable >::size_type i(0); i < variables_.size(); ++i )
    {
        if( variables_[i].name() == name_ + "." + name )
            return variables_[i];
    }
    throw uniformblock_error( name + std::string(" variable not found") );
}
