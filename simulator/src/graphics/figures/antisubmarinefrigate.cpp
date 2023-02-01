/* 
 * File:   antisubmarinefrigate.cpp
 * Author: mkh
 * 
 * Created on 31 января 2023 г., 14:45
 */

#include "antisubmarinefrigate.h"
#include "../../../../share/utils.h"
#include "../program.h"
#include "../scene.h"

namespace
{
char const shader_name[] = "military-anti-submarine-frigate-warship-vessel-silhouette.glsl";
char const texture_name[] = "/military-anti-submarine-frigate-warship-vessel-silhouette.png";
}

antisubmarinefrigate::antisubmarinefrigate()
{
}

antisubmarinefrigate::~antisubmarinefrigate()
{
}

void antisubmarinefrigate::initialize()
{
    program_.reset(new program( shader_name ) );
    texture_.reset( new texture( (std::string(utils::config()["textures"]) + texture_name).c_str() ) );
    
    glBufferData( GL_ARRAY_BUFFER, sizeof(position_), position_, GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW); 
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0 );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray( 1 );   
}

void antisubmarinefrigate::draw( double currentTime )
{
    glUseProgram( *program_ );
    texture_->activate();
    set_attribute( "Model", model_ );
    set_attribute( "Texture", GLuint(0) );
    set_attribute( "Offset", offset_ );
    set_attribute( "Background", glm::vec4(0.2f, 0.3f, 0.3f,  1.0f) );
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
    if( scene_position_ < -1.0 || scene_position_ > 1.0 )
    {
        offset_.x = 0.0f;
        direction_ *= -1;
        angle_ = angle_ == 0.0f ? 180.0f : 0.0f;
        model_ = glm::mat4( glm::translate(
                                   glm::rotate( glm::mat4(1.0f),
                                                glm::radians( angle_ ),
                                                glm::vec3( 0.0f, 1.0f, 0.0f ) ),
                                   glm::vec3( 1.0f, 0.0f, 0.0f ) ) );
    }
    scene_position_ += speed_ * direction_;
    offset_.x -= speed_;
}
