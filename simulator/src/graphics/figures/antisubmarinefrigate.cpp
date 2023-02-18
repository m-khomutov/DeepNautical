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

bool antisubmarinefrigate::environment_valid()
{
    return utils::file_exists( (std::string(utils::config()["shaders"]) + "/vert_" + shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["shaders"]) + "/frag_" + shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["textures"]) + texture_name).c_str() );
}

antisubmarinefrigate::antisubmarinefrigate()
{
    speed_ = 0.005f;
    f_set_model();
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
    if( scene_position_ < -1.5 || scene_position_ > 1.5 )
    {
        offset_.x = 0.0f;
        direction_ *= -1;
        angle_ = angle_ == 0.0f ? 180.0f : 0.0f;
        f_set_model();
    }
    scene_position_ += speed_ * direction_;
    offset_.x -= speed_;
    
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

void antisubmarinefrigate::f_set_model()
{
    GLfloat y = angle_ == 0.0f ? -0.08f : -0.38f;
    glm::vec3 factor = angle_ == 0.0f ? glm::vec3( 0.7f ) : glm::vec3( 1.0f );
    model_ = glm::mat4( glm::scale(
                            glm::translate(
                                glm::rotate( glm::mat4(1.0f),
                                             glm::radians( angle_ ),
                                             glm::vec3( 0.0f, 1.0f, 0.09f ) ),
                                glm::vec3( 1.0f, y, 0.0f ) ),
                            factor ) );
}
