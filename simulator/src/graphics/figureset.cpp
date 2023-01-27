/* 
 * File:   figure_set.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 14:52
 */

#include "figureset.h"

figureset::figureset()
{
}

figureset::~figureset()
{
}

void figureset::set_vao( GLuint vao )
{
    glBufferData( GL_ARRAY_BUFFER, sizeof(triangles_), triangles_, GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0 );
    glEnableVertexAttribArray( 0 );
}
