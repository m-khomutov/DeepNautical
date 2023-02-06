/* 
 * File:   antisubmarinefrigate.cpp
 * Author: mkh
 * 
 * Created on 31 января 2023 г., 14:45
 */

#include "antisubmarinefrigate.h"

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

char const *antisubmarinefrigate::f_shader_name() const
{
    return shader_name; 
}

void antisubmarinefrigate::f_initialize()
{
    texture_.reset( new texture( (std::string(utils::config()["textures"]) + texture_name).c_str() ) );
    
    glBufferData( GL_ARRAY_BUFFER, sizeof(position_), position_, GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW); 
    GLuint index = program_->attribute_index( "position" );
    glVertexAttribPointer( index, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0 );
    glEnableVertexAttribArray( index );
    index = program_->attribute_index( "texcoord" );
    glVertexAttribPointer( index, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray( index );   
}

void antisubmarinefrigate::f_draw( double currentTime )
{
    set_attribute( "Texture", GLuint(0) );
    set_attribute( "Offset", offset_ );
    set_attribute( "Waterline", waterline_ );
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
    
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}
