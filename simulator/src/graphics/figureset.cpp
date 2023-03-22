/* 
 * File:   figure_set.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 14:52
 */

#include "figureset.h"
#include <glm/gtc/type_ptr.hpp>

figureset::figureset()
{
}

figureset::~figureset()
{
    if( ! figures_.empty() )
    {
        glDeleteVertexArrays( vao_.size(), vao_.data() );
        glDeleteBuffers( vbo_.size(), vbo_.data() );
        glDeleteBuffers( ebo_.size(), ebo_.data() );
    }
}

void figureset::emplace( figure *fig )
{
    figures_.emplace_back( fig );
}

void figureset::initialize()
{
    if( figures_.empty() )
    {
        return;
    }
    vao_.resize( figures_.size() );
    vbo_.resize( figures_.size() );
    ebo_.resize( figures_.size() );
    

    glGenVertexArrays( vao_.size(), vao_.data() );
    glGenBuffers( vbo_.size(), vbo_.data() );
    glGenBuffers( ebo_.size(), ebo_.data() );
    
    for( figure_t::size_type i(0); i < figures_.size(); ++i ) {
        glBindVertexArray( vao_[i] );
        glBindBuffer( GL_ARRAY_BUFFER, vbo_[i] );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo_[i] );
        try
        {
            figures_[i]->initialize();
        }
        catch( const std::runtime_error& err )
        {
            std::cerr << typeid(*figures_[i]).name() << " error: " << err.what() <<std::endl;
        }
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
    }
}

void figureset::draw( double currentTime )
{
    for( figure_t::size_type i(0); i < figures_.size(); ++i )
    {
        if( figures_[i]->valid() )
        {
           glBindVertexArray( vao_[i] );
            figures_[i]->draw( currentTime );
            glBindVertexArray( 0 );
        }
    }
}
