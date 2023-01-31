/* 
 * File:   figure_set.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 14:52
 */

#include <glm/gtc/type_ptr.hpp>

#include "figureset.h"
#include "program.h"
#include "scene.h"
#include "figures/antisubmarinefrigate.h"
#include <iostream> 

figureset::figureset()
{
}

figureset::~figureset()
{
}

void figureset::initialize( program &prog )
{
    figures_.emplace_back( new antisubmarinefrigate );
    for( auto f : figures_ )
    {
	f->initialize( prog );
    }
}

void figureset::draw( scene &sc, double currentTime )
{
    for( auto f : figures_ )
    {
	f->draw( sc, currentTime );
    }
}
