/* 
 * File:   figure_set.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 14:52
 */

#include <glm/gtc/type_ptr.hpp>

#include "figureset.h"

figureset::figureset()
{
}

figureset::~figureset()
{
}

void figureset::set_vbo( GLuint vbo )
{
    if( vbo == 0 )
    {
        glBufferData( GL_ARRAY_BUFFER, sizeof(position_), position_, GL_STATIC_DRAW );
    }
    else
    {
        glBufferData( GL_ARRAY_BUFFER, sizeof(color_), color_, GL_STATIC_DRAW );
    }
    glVertexAttribPointer( vbo, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0 );
    glEnableVertexAttribArray( vbo );
}
