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

void uniformblock::emplace_variable( std::string const &name, GLint index )
{
    variables_.emplace_back( name.c_str(), index );
}

void uniformblock::copy()
{
    glBindBuffer( GL_UNIFORM_BUFFER, ubo_ );
    
    const GLchar *names[variables_.size()];
    for( GLuint i(0); i < variables_.size(); ++i )
        names[i] = variables_[i].name().c_str();
    GLuint indices[variables_.size()];
    glGetUniformIndices( program_id_, variables_.size(), names, indices );
    GLint offset[variables_.size()];
    glGetActiveUniformsiv( program_id_, variables_.size(), indices, GL_UNIFORM_OFFSET, offset );
    for( GLuint i(0); i < variables_.size(); ++i )
        ::memcpy( buffer_.data() + offset[i], variables_[i], variables_[i].size() );
    glBufferData( GL_UNIFORM_BUFFER, buffer_.size(), buffer_.data(), GL_DYNAMIC_DRAW );
    glBindBufferBase( GL_UNIFORM_BUFFER, 0, ubo_ );
}

uniformblock::variable &uniformblock::operator []( GLchar const *name )
{
    for( int i(0); i < variables_.size(); ++i )
    {
        if( variables_[i].name() == name_ + "." + name )
            return variables_[i];
    }
    throw uniformblock_error( name + std::string(" variable not found") );
}
