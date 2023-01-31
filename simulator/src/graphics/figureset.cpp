/* 
 * File:   figure_set.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 14:52
 */

#include <glm/gtc/type_ptr.hpp>

#include "figureset.h"
#include "../../../share/utils.h"
#include "program.h"
#include "scene.h"
#include <iostream> 

figureset::figureset()
{
}

figureset::~figureset()
{
}

void figureset::initialize( program &prog )
{
    texture_.reset( new texture( (std::string(utils::config()["textures"]) + 
                                  "/military-anti-submarine-frigate-warship-vessel-silhouette.png"
                                 ).c_str() ) );

    prog.uniform_block("Circle" )["InnerColor"] = glm::vec4(1.0f, 1.0f, 0.75f, 1.0f);
    prog.uniform_block("Circle" )["OuterColor"] = glm::vec4(0.2f, 0.3f, 0.3f,  1.0f);
    prog.uniform_block("Circle" )["InnerRadius"] = 0.25f;
    prog.uniform_block("Circle" )["OuterRadius"] = 0.45f;
    prog.uniform_block("Circle").copy();

    glBufferData( GL_ARRAY_BUFFER, sizeof(position_), position_, GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW); 
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0 );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray( 1 );
}

void figureset::draw( scene &sc, double currentTime )
{
    //set_attribute( 0, "RotationMatrix", rotation_ );
    texture_->activate();
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
    sc.set_attribute( "Texture", GLuint(0) );
    sc.set_attribute( "offset", offset_ );
    if( offset_ < -1.0 || offset_ > 1.0 )
    {
	offset_inc_ *= -1;
    }
    offset_ += offset_inc_;
}
