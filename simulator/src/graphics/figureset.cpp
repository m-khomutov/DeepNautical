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
{}

figureset::~figureset()
{
    if( ! figures_.empty() )
    {
        glDeleteVertexArrays( vao_.size(), vao_.data() );
        glDeleteBuffers( vbo_.size(), vbo_.data() );
        glDeleteBuffers( ebo_.size(), ebo_.data() );
    }
}

void figureset::emplace( TFigure *fig )
{
    figures_.emplace_back( fig );
    vbo_count_ += fig->vbo_count();
}

TFigure *figureset::back()
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
    vbo_.resize( vbo_count_ );
    ebo_.resize( figures_.size() );
    

    glGenVertexArrays( vao_.size(), vao_.data() );
    glGenBuffers( vbo_.size(), vbo_.data() );
    glGenBuffers( ebo_.size(), ebo_.data() );
    
    GLuint *p_vbo = vbo_.data();
    for( figure_t::size_type i(0); i < figures_.size(); ++i ) {
        glBindVertexArray( vao_[i] );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo_[i] );
        for( size_t bc(0); bc < figures_[i]->vbo_count(); ++bc )
        {
            glBindBuffer( GL_ARRAY_BUFFER, *p_vbo );
            try
            {
                figures_[i]->initialize( bc );
            }
            catch( const std::runtime_error& err )
            {
                std::cerr << typeid(*figures_[i]).name() << " error: " << err.what() <<std::endl;
            }
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            p_vbo ++;
        }
        glBindVertexArray( 0 );
    }
}

void figureset::draw( double currentTime )
{
    GLuint *p_vbo = vbo_.data();
    for( figure_t::size_type i(0); i < figures_.size(); ++i )
    {
        if( figures_[i]->valid() )
        {
            glBindVertexArray( vao_[i] );
            for( size_t bc(0); bc < figures_[i]->vbo_count(); ++bc )
            {
                glBindBuffer( GL_ARRAY_BUFFER, *p_vbo );
                figures_[i]->accept( bc, *this, currentTime );
                glBindBuffer( GL_ARRAY_BUFFER, 0 );
                p_vbo ++;
            }
            glBindVertexArray( 0 );
        }
    }
}

void figureset::visit( size_t vbo_number, TAntisubmarinefrigate *frigate )
{
    frigate->draw( vbo_number );
}

void figureset::visit( size_t vbo_number, sol *_sol )
{
    _sol->draw( vbo_number );
}

void figureset::visit( size_t vbo_number, water *_water )
{
    _water->set_wake_position( vessel_positions_ );
    _water->draw( vbo_number );
    vessel_positions_.clear();
}

void figureset::visit( size_t vbo_number, horizon *_horizon )
{
    _horizon->draw( vbo_number );
}

void figureset::visit( size_t vbo_number, vessel *_vessel )
{
    _vessel->draw( vbo_number );
    vessel_positions_.push_back( _vessel->position() );
}


void figureset::visit( size_t vbo_number, sparklets *_sparklets )
{
    _sparklets->draw( vbo_number );
}
