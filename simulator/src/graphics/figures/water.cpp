/* 
 * File:   water.cpp
 * Author: mkh
 * 
 * Created on 1 февраля 2023 г., 14:18
 */

#include "water.h"

namespace
{
char const shader_name[] = "water.glsl";
char const texture_name[] = "/water.png";
}

bool water::environment_valid()
{
    return utils::file_exists( (std::string(utils::config()["shaders"]) + "/vert_" + shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["shaders"]) + "/frag_" + shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["textures"]) + texture_name).c_str() );
}

water::water()
{
}

water::~water()
{
}

char const *water::f_shader_name() const
{
    return shader_name; 
}

void water::f_initialize()
{
    texture_.reset( new texture( (std::string(utils::config()["textures"]) + texture_name).c_str() ) );

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
