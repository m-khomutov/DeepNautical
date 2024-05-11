/* 
 * File:   figure_set.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 14:52
 */

#include "figureset.h"
#include "figures/sol.h"
#include "figures/water.h"
/*#include "figures/antisubmarinefrigate.h"
#include "figures/horizon.h"
#include "figures/vessel.h"
#include "figures/sparklets.h"*/
#include <glm/gtc/type_ptr.hpp>

TFigureset::~TFigureset()
{
    if( !figures_.empty() )
    {
        //glDeleteVertexArrays( vao_.size(), vao_.data() );
        //glDeleteBuffers( vbo_.size(), vbo_.data() );
        //glDeleteBuffers( ebo_.size(), ebo_.data() );
    }
}

void TFigureset::emplace( TFigure *fig )
{
    //добавить фигуру
    figures_.emplace_back( fig );
    // посчитать используемые ею VBO
    vbo_count_ += figures_.back()->vbo_count();
}

void TFigureset::clear()
{
    figures_.clear();
}

void TFigureset::initialize()
{
    if( figures_.empty() )
    {
        return;
    }
    // выделить ресурсы под буферы взаимодействия с шейдерами
    vao_.resize( figures_.size() );
    vbo_.resize( vbo_count_ );
    ebo_.resize( figures_.size() );
    

    //glGenVertexArrays( vao_.size(), vao_.data() );
    //glGenBuffers( vbo_.size(), vbo_.data() );
    //glGenBuffers( ebo_.size(), ebo_.data() );

    GLuint *p_vbo = vbo_.data();
    // проинициализировать объекты фигур
    for( figure_t::size_type i(0); i < figures_.size(); ++i ) {
        // у каждой фигуры свои буферы
        //glBindVertexArray( vao_[i] );
        //glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo_[i] );
        for( size_t bc(0); bc < figures_[i]->vbo_count(); ++bc )
        {
            //glBindBuffer( GL_ARRAY_BUFFER, *p_vbo );
            try
            {
                figures_[i]->initialize( bc );
            }
            catch( const std::runtime_error& err )
            {
                std::cerr << typeid(*figures_[i]).name() << " error: " << err.what() <<std::endl;
            }
            //glBindBuffer( GL_ARRAY_BUFFER, 0 );
            p_vbo ++;
        }
        //glBindVertexArray( 0 );
    }
}

void TFigureset::draw( double currentTime )
{
    GLuint *p_vbo = vbo_.data();
    for( figure_t::size_type i(0); i < figures_.size(); ++i )
    {
        if( figures_[i]->valid() )
        {
            // привязать нужные буферы работы с шейдерами
            //glBindVertexArray( vao_[i] );
            for( size_t bc(0); bc < figures_[i]->vbo_count(); ++bc )
            {
                //glBindBuffer( GL_ARRAY_BUFFER, *p_vbo );
                // запустить метод принятия посетителя (себя)
                figures_[i]->accept( bc, *this, currentTime );
                //glBindBuffer( GL_ARRAY_BUFFER, 0 );
                p_vbo ++;
            }
            //glBindVertexArray( 0 );
        }
    }
}

void TFigureset::visit( size_t vbo_number, TSol *_sol )
{
    _sol->draw( vbo_number );
}

void TFigureset::visit( size_t vbo_number, TWater *_water )
{
    _water->set_wake_position( vessel_positions_ );
    _water->draw( vbo_number );

    vessel_positions_.clear();
}

/*void TFigureset::visit( size_t vbo_number, TAntisubmarinefrigate *frigate )
{
    frigate->draw( vbo_number );
}

void TFigureset::visit( size_t vbo_number, THorizon *_horizon )
{
    _horizon->draw( vbo_number );
}

void TFigureset::visit( size_t vbo_number, TVessel *_vessel )
{
    _vessel->draw( vbo_number );
    vessel_positions_.push_back( _vessel->position() );
}


void TFigureset::visit( size_t vbo_number, TSparklets *_sparklets )
{
    _sparklets->draw( vbo_number );
}
*/
