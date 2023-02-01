/* 
 * File:   antisubmarinefrigate.cpp
 * Author: mkh
 * 
 * Created on 31 января 2023 г., 14:45
 */
#include <GL/glew.h>
#include "antisubmarinefrigate.h"
#include "../../../../share/utils.h"
#include "../program.h"
#include "../scene.h"

namespace
{
char const texture_name[] = "/military-anti-submarine-frigate-warship-vessel-silhouette.png";
}

antisubmarinefrigate::antisubmarinefrigate()
{
}

antisubmarinefrigate::~antisubmarinefrigate()
{
}

void antisubmarinefrigate::initialize( program &prog )
{
    texture_.reset( new texture( (std::string(utils::config()["textures"]) + texture_name).c_str() ) );

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

void antisubmarinefrigate::draw( scene &sc, double currentTime )
{
    texture_->activate();
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
    sc.set_attribute( "RotationMatrix", rotation_ );
    sc.set_attribute( "Texture", GLuint(0) );
    sc.set_attribute( "Offset", offset_ );
    if( scene_position_ < -1.0 || scene_position_ > 1.0 )
    {
        offset_.x = 0.0f;
        direction_ *= -1;
        angle_ = angle_ == 0.0f ? 180.0f : 0.0f;
        rotation_ = glm::mat4( glm::translate(
                                   glm::rotate( glm::mat4(1.0f),
                                                glm::radians( angle_ ),
                                                glm::vec3( 0.0f, 1.0f, 0.0f ) ),
                                   glm::vec3( 1.0f, 0.0f, 0.0f ) ) );
    }
    scene_position_ += speed_ * direction_;
    offset_.x -= speed_;
}
