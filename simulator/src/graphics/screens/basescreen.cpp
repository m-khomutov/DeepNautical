
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
    utils::read_directory( utils::config()["scenes"],
                           ".scn",
                           [this]( const std::string &name )
                           {
                               scenes_.insert( name.substr( 0, name.size() - 4 ) );
                           } );
    if( scenes_.empty() )
        throw screen_error( std::string("no scenes found") );
    
    scene_iter_ = scenes_.begin();
}

basescreen::~basescreen()
{
}

void basescreen::set_scene( const std::string &scene )
{
    if( scene == *scene_iter_ )
    {
        return;
    }
    std::set< std::string>::iterator it = scenes_.find( scene );
    if( it == scenes_.end() )
    {
        throw screen_error( scene + " not found" );
    }
    scene_iter_ = it;

    auto g = commands_.get();
    g->emplace_back( command::set_scene );
}

void basescreen::f_exec_command()
{
    auto g = commands_.get();
    if( ! g->empty() )
    {
        command cmd = g->front();
        g->pop_front();
        switch( cmd.type() )
        {
        case command::set_scene:
            sc_.reset(new scene( std::string(utils::config()["scenes"]) + "/" + *scene_iter_ + ".scn" ) );
            break;
        }
    }
}
