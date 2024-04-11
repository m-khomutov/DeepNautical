/* 
 * File:   sol.cpp
 * Author: mkh
 * 
 * Created on 1 февраля 2023 г., 10:12
 */

#include "sol.h"
#include <glm/gtx/transform.hpp>


sol::sol( const std::vector< std::string > &settings, const glm::vec3 &camera_pos )
: figure( settings, camera_pos )
{
    offset_ = glm::vec3( -2.5f, 2.0f, 0.0f );
    model_ = glm::mat4( glm::scale( 
                            glm::rotate(
                                glm::translate( glm::vec3(0.0f, 0.0f, -1.0f) ),
                                glm::radians( 50.0f ), glm::vec3(1.0f, 0.0f, 0.0f) ),
                            glm::vec3( 0.3f ) ) );
}

sol::~sol()
{
}

void sol::draw( size_t )
{
    set_uniform( "Offset", offset_ );
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

void sol::f_check_environment() const
{
    if( ! (NUtils::file_exists( (std::string(NUtils::config()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::config()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::config()["textures"]) + "/" + spec_.texture_name).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + "}"  );
    }
}

char const *sol::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void sol::f_initialize( size_t )
{
    texture_.reset( new texture( (std::string(NUtils::config()["textures"]) + "/" + spec_.texture_name).c_str() ) );

    glBufferData( GL_ARRAY_BUFFER, spec_.viewport.size() * sizeof(float), spec_.viewport.data(), GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW); 
    set_attribute( "Position", 3, 5, 0 );
    set_attribute( "Texcoord", 2, 5, 3 );
    set_uniform( "CameraPosition", spec_.camera_position );
    set_uniform( "Fog.color", spec_.fog_color );
    set_uniform( "Fog.density", spec_.fog_density );
    program_->uniform_block("CircleParams")["OuterColor"].set( glm::vec3(0.392f, 0.706f, 0.983f) );
    program_->uniform_block("CircleParams")["InnerColor"].set( glm::vec3(1.0f, 1.0f, 0.75f) );
    program_->uniform_block("CircleParams")["OuterColor"].set( glm::vec3(0.392f, 0.706f, 0.983f) );
    program_->uniform_block("CircleParams")["InnerRadius"].set( 0.25f );
    program_->uniform_block("CircleParams")["OuterRadius"].set( 0.45f );
    program_->uniform_block("CircleParams").copy();
}

void sol::f_accept( size_t vbo_number, visitor &p, double )
{
    p.visit( vbo_number, this );
}
