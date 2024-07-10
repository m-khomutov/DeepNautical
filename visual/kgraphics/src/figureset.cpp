/* 
 * File:   figure_set.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 14:52
 */

#include "figureset.h"
#include "figures/air.h"
#include "figures/waves.h"
#include "figures/surge.h"
#include "figures/vessel.h"
#include "figures/skybox.h"

#include <limits>

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

const TFigure* TFigureset::closest_on_distance( const CU& point, double distance )
{
    const TFigure *rc = nullptr;
    double min_distance = std::numeric_limits< double >::max();
    for( auto & fig : figures_ )
    {
        if( fig->moving() )
        {
            double d = fig->distance( point );
            if( d < min_distance )
            {
                rc = fig.get();
                min_distance = d;
            }
        }
    }
    return min_distance <= distance ? rc : nullptr;
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

void TFigureset::draw( double currentTime, const TCamera &camera )
{
    for( figure_t::size_type i(0); i < figures_.size(); ++i )
    {
        if( figures_[i]->valid() )
        {
            // запустить метод принятия посетителя (себя)
            figures_[i]->accept( *this, currentTime, camera );
        }
    }
}

void TFigureset::visit( TAir *_air )
{
    _air->draw();
}

void TFigureset::visit( TWaves *_waves )
{
    _waves->set_wake_position( vessel_positions_ );
    _waves->draw();

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

void TFigureset::visit( TSkybox *_skybox )
{
    _skybox->draw();
}

