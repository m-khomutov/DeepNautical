/* 
 * File:   antisubmarinefrigate.cpp
 * Author: mkh
 * 
 * Created on 31 января 2023 г., 14:45
 */

#include "antisubmarinefrigate.h"

antisubmarinefrigate::antisubmarinefrigate( const std::vector< std::string > &settings, const glm::vec3 &camera_pos )
: figure( settings, camera_pos )
{
    f_set_model();
}

antisubmarinefrigate::~antisubmarinefrigate()
{
}

void antisubmarinefrigate::draw( size_t )
{
    set_uniform( "Texture", GLuint(0) );
    set_uniform( "Offset", offset_ );
    set_uniform( "Waterline", waterline_ );
    if( scene_position_ < -1.5 || scene_position_ > 1.5 )
    {
        offset_.x = 0.0f;
        direction_ *= -1;
        angle_.x = angle_.x == 0.0f ? 180.0f : 0.0f;
        f_set_model();
    }
    scene_position_ += spec_.speed.x * direction_;
    offset_.x -= spec_.speed.x;
    
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );   
}

void antisubmarinefrigate::f_check_environment() const
{
    if( ! (utils::file_exists( (std::string(utils::config()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["textures"]) + "/" + spec_.texture_name).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + " " + spec_.texture_name + "}"  );
    }
}

char const *antisubmarinefrigate::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void antisubmarinefrigate::f_initialize( size_t )
{
    texture_.reset( new texture( (std::string(utils::config()["textures"]) + spec_.texture_name).c_str() ) );
    
    glBufferData( GL_ARRAY_BUFFER, sizeof(position_), position_, GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW); 
    set_attribute( "Position", 3, 5, 0 );
    set_attribute( "Texcoord", 2, 5, 3 );
}

void antisubmarinefrigate::f_accept( size_t vbo_number, visitor &p, double )
{
    p.visit( vbo_number, this );
}

void antisubmarinefrigate::f_set_model()
{
    GLfloat y = angle_.x == 0.0f ? -0.08f : -0.38f;
    glm::vec3 factor = angle_.x == 0.0f ? glm::vec3( 0.7f ) : glm::vec3( 1.0f );
    model_ = glm::mat4( glm::scale(
                            glm::translate(
                                glm::rotate( glm::mat4(1.0f),
                                             glm::radians( angle_.x ),
                                             glm::vec3( 0.0f, 1.0f, 0.09f ) ),
                                glm::vec3( 1.0f, y, 0.0f ) ),
                            factor ) );
}
