/* 
 * File:   water.cpp
 * Author: mkh
 * 
 * Created on 1 февраля 2023 г., 14:18
 */

#include "water.h"
#include "../program.h"
#include "../../../../share/utils.h"

namespace
{
char const shader_name[] = "water.glsl";
char const texture_name[] = "/water.png";
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
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0 );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray( 1 );   
}

void water::f_draw( double currentTime )
{
    set_attribute( "Model", model_ );
    set_attribute( "Offset", offset_ );
    
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}
