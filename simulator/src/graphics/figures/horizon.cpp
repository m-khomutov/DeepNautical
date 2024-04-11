/* 
 * File:   horizon.cpp
 * Author: mkh
 * 
 * Created on 13 февраля 2023 г., 17:58
 */

#include "horizon.h"

horizon::horizon( const std::vector< std::string > &settings, const glm::vec3 &camera_pos )
: figure( settings, camera_pos )
{
    f_check_environment();
    avi_.reset( new avi( (std::string(NUtils::config()["textures"]) + "/" + spec_.texture_name).c_str() ) );
}

horizon::~horizon()
{
}

void horizon::draw( size_t )
{
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

void horizon::f_check_environment() const
{
    if( ! (NUtils::file_exists( (std::string(NUtils::config()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::config()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::config()["textures"]) + "/" + spec_.texture_name).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + " " + spec_.texture_name + "}"  );
    }
}

char const *horizon::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void horizon::f_initialize( size_t )
{
    texture_.reset( new texture( avi_->next_image() ) );

    glBufferData( GL_ARRAY_BUFFER, spec_.viewport.size() * sizeof(float), spec_.viewport.data(), GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW); 
    set_attribute( "Position", 3, 5, 0 );
    set_attribute( "Texcoord", 2, 5, 3 );
}

void horizon::f_accept( size_t vbo_number, visitor &p, double current_time )
{
    if( last_frame_time_ < 0.01f )
    {
        last_frame_time_ = current_time;
    }
    if( current_time - last_frame_time_ >= avi_->frame_duration() )
    {
        *texture_ = avi_->next_image();
        last_frame_time_ = current_time;
    }
    p.visit( vbo_number, this );
}
