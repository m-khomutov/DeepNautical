/* 
 * File:   figure_set.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 14:52
 */

#include "figureset.h"
#include "figures/antisubmarinefrigate.h"
#include "figures/sol.h"
#include "figures/water.h"
#include "figures/horizon.h"
#include "figures/vessel.h"
#include "figures/sparklets.h"
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

figure *figureset::back()
{
    return figures_.back().get();
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
            glBindBuffer( GL_ARRAY_BUFFER, vbo_[i] );
            figures_[i]->accept( *this, currentTime );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );
        }
    }
}

void figureset::visit( antisubmarinefrigate *frigate )
{
    frigate->draw();
}

void figureset::visit( sol *_sol )
{
    _sol->draw();
}

void figureset::visit( water *_water )
{
    _water->set_wake_position( vessel_positions_ );
    _water->draw();
    vessel_positions_.clear();
}

void figureset::visit( horizon *_horizon )
{
    _horizon->draw();
}

void figureset::visit( vessel *_vessel )
{
    _vessel->draw();
    vessel_positions_.push_back( _vessel->position() );
}


void figureset::visit( sparklets *_sparklets )
{
    _sparklets->draw();
}
