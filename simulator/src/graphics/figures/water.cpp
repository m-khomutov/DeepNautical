/* 
 * File:   water.cpp
 * Author: mkh
 * 
 * Created on 1 февраля 2023 г., 14:18
 */

#include "water.h"

water::water( const std::vector< std::string > &settings )
: figure( settings )
{
}

water::~water()
{
}
void water::f_check_environment() const
{
    if( ! (utils::file_exists( (std::string(utils::config()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["textures"]) + "/" + spec_.texture_name).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + " " + spec_.texture_name + "}"  );
    }
}

char const *water::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void water::f_initialize()
{
    texture_.reset( new texture( (std::string(utils::config()["textures"]) + spec_.texture_name).c_str() ) );

    glBufferData( GL_ARRAY_BUFFER, sizeof(position_), position_, GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW); 
    set_layout( "position", 3, 5, 0 );
    set_layout( "texcoord", 2, 5, 3 );
}

void water::f_draw( double currentTime )
{
    set_attribute( "Offset", offset_ );

    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}
