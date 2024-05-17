/* 
 * File:   specification.cpp
 * Author: mkh
 * 
 * Created on 1 марта 2023 г., 10:23
 */

#include "specification.h"
#include "utils.h"
#include <iostream>

TSpecification::TSpecification( const NJson::TObject &environment,const NJson::TObject &settings )
{
    try
    {
        camera_position.setX( environment["camera-position"]["x"].toFloat() );
        camera_position.setY( environment["camera-position"]["y"].toFloat() );
        camera_position.setZ( environment["camera-position"]["z"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        light_color.setX( environment["light-color"]["r"].toFloat() );
        light_color.setY( environment["light-color"]["g"].toFloat() );
        light_color.setZ( environment["light-color"]["b"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        light_position.setX( environment["light-position"]["x"].toFloat() );
        light_position.setY( environment["light-position"]["y"].toFloat() );
        light_position.setZ( environment["light-position"]["z"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        fog_color.setX( environment["fog-color"]["r"].toFloat() );
        fog_color.setY( environment["fog-color"]["g"].toFloat() );
        fog_color.setZ( environment["fog-color"]["b"].toFloat() );
        fog_color.setW( environment["fog-color"]["a"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        fog_density = environment["fog-density"].toFloat();
    }
    catch( const std::exception &e )
    {}


    try
    {
        shader_name = std::string(settings["shader"]);
    }
    catch( const std::exception &e )
    {}
    try
    {
        texture_name = std::string(settings["main-texture"]);
    }
    catch( const std::exception &e )
    {}
    try
    {
        texture_alpha = std::string(settings["alpha-texture"]);
    }
    catch( const std::exception &e )
    {}
    try
    {
        texture_air = std::string(settings["air-texture"]);
    }
    catch( const std::exception &e )
    {}
    try
    {
        texture_foam = std::string(settings["foam-texture"]);
    }
    catch( const std::exception &e )
    {}
    try
    {
        blenderobj_name = std::string(settings["blenderobj-name"]);
    }
    catch( const std::exception &e )
    {}
    try
    {
        speed.setX( settings["speed"]["x"].toFloat() );
        speed.setY( settings["speed"]["y"].toFloat() );
        speed.setZ( settings["speed"]["z"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        pitching_range.setX( settings["pitching-range"]["x"].toFloat() );
        pitching_range.setY( settings["pitching-range"]["y"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        start_position.setX( settings["start-position"]["x"].toFloat() );
        start_position.setY( settings["start-position"]["y"].toFloat() );
        start_position.setZ( settings["start-position"]["z"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        start_factor.setX( settings["start-factor"]["x"].toFloat() );
        start_factor.setY( settings["start-factor"]["y"].toFloat() );
        start_factor.setZ( settings["start-factor"]["z"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        factor_gain.setX( settings["factor-gain"]["x"].toFloat() );
        factor_gain.setY( settings["factor-gain"]["y"].toFloat() );
        factor_gain.setZ( settings["factor-gain"]["z"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        angle_gain.setX( settings["angle-gain"]["x"].toFloat() );
        angle_gain.setY( settings["angle-gain"]["y"].toFloat() );
        angle_gain.setZ( settings["angle-gain"]["z"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        trajectory = settings["trajectory"].toFloat();
    }
    catch( const std::exception &e )
    {}
    try
    {
        course.setX( settings["course"]["x"].toFloat() );
        course.setY( settings["course"]["y"].toFloat() );
        course.setZ( settings["course"]["z"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        pitching = settings["pitching"].toFloat();
    }
    catch( const std::exception &e )
    {}
    try
    {
        viewport.push_back( settings["viewport"]["bottomleft"]["x"].toFloat() );
        viewport.push_back( settings["viewport"]["bottomleft"]["y"].toFloat() );
        viewport.push_back( settings["viewport"]["bottomleft"]["z"].toFloat() );
        viewport.push_back( 0.f );
        viewport.push_back( 0.f );
        viewport.push_back( settings["viewport"]["bottomright"]["x"].toFloat() );
        viewport.push_back( settings["viewport"]["bottomright"]["y"].toFloat() );
        viewport.push_back( settings["viewport"]["bottomright"]["z"].toFloat() );
        viewport.push_back( 1.f );
        viewport.push_back( 0.f );
        viewport.push_back( settings["viewport"]["topright"]["x"].toFloat() );
        viewport.push_back( settings["viewport"]["topright"]["y"].toFloat() );
        viewport.push_back( settings["viewport"]["topright"]["z"].toFloat() );
        viewport.push_back( 1.f );
        viewport.push_back( 1.f );
        viewport.push_back( settings["viewport"]["topleft"]["x"].toFloat() );
        viewport.push_back( settings["viewport"]["topleft"]["y"].toFloat() );
        viewport.push_back( settings["viewport"]["topleft"]["z"].toFloat() );
        viewport.push_back( 0.f );
        viewport.push_back( 1.f );
    }
    catch( const std::exception &e )
    {}
    try
    {
        ripple[0].setX( settings["ripple"]["amplitude"]["x"].toFloat() );
        ripple[0].setY( settings["ripple"]["amplitude"]["y"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        ripple[0].setX( settings["ripple"]["frequency"]["x"].toFloat() );
        ripple[0].setY( settings["ripple"]["frequency"]["y"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        step = settings["step"].toFloat();
    }
    catch( const std::exception &e )
    {}
    try
    {
        wave.setX( settings["wave"]["x"].toFloat() );
        wave.setY( settings["wave"]["y"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        wake.setX( settings["wake"]["x"].toFloat() );
        wake.setY( settings["wake"]["y"].toFloat() );
    }
    catch( const std::exception &e )
    {}
    try
    {
        wake.setX( settings["wake-width"]["x"].toFloat() );
        wake.setY( settings["wake-width"]["y"].toFloat() );
    }
    catch( const std::exception &e )
    {}
}
