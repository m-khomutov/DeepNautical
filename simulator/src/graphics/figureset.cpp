/* 
 * File:   figure_set.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 14:52
 */

#include "figureset.h"
#include "figures/sol.h"
#include "figures/water.h"
#include "figures/surge.h"
#include "figures/vessel.h"

TFigureset::~TFigureset()
{}

void TFigureset::emplace( TFigure *fig )
{
    //добавить фигуру
    figures_.emplace_back( fig );
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
    // проинициализировать объекты фигур
    for( figure_t::size_type i(0); i < figures_.size(); ++i )
    {
        try
        {
            figures_[i]->initialize();
        }
        catch( const std::runtime_error& err )
        {
            qDebug() << typeid(*figures_[i]).name() << " error: " << err.what();
        }
    }
}

void TFigureset::draw( double currentTime )
{
    for( figure_t::size_type i(0); i < figures_.size(); ++i )
    {
        if( figures_[i]->valid() )
        {
            // запустить метод принятия посетителя (себя)
            figures_[i]->accept( *this, currentTime );
        }
    }
}

void TFigureset::visit( TSol *_sol )
{
    _sol->draw();
}

void TFigureset::visit( TWater *_water )
{
    _water->set_wake_position( vessel_positions_ );
    _water->draw();

    vessel_positions_.clear();
}

void TFigureset::visit( TSurge *_surge )
{
    _surge->draw();
}

void TFigureset::visit( TVessel *_vessel )
{
    _vessel->draw();
    vessel_positions_.push_back( _vessel->position() );
}
