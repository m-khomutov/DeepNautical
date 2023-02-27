
/* 
 * File:   base.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 14:25
 */

#include "basescreen.h"

screen_error::screen_error( const std::string &what )
: std::runtime_error( what )
{
}

basescreen::basescreen()
{
    utils::read_directory( utils::config()["scenes"], ".scn", [this]( const std::string &name ){ scenes_.push_back( name ); } );
    if( scenes_.empty() )
        throw screen_error( std::string("no scenes found") );
    
    scene_iter_ = scenes_.begin();
}

basescreen::~basescreen()
{
}
