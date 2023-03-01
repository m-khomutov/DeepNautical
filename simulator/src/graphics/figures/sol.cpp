/* 
 * File:   sol.cpp
 * Author: mkh
 * 
 * Created on 1 февраля 2023 г., 10:12
 */

#include "sol.h"
#include <glm/gtx/transform.hpp>


sol::sol( const std::vector< std::string > &settings )
: figure( settings )
{
    offset_ = glm::vec3( -2.5f, 2.0f, 0.0f );
    model_ = glm::mat4( glm::scale( 
                            glm::translate( glm::vec3(0.0f, 0.0f, -1.0f) ),
                            glm::vec3( 0.3f ) ) );
}

sol::~sol()
{
}

void sol::f_check_environment() const
{
    if( ! (utils::file_exists( (std::string(utils::config()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) ) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + "}"  );
    }
}

char const *sol::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void sol::f_initialize()
{
    glBufferData( GL_ARRAY_BUFFER, sizeof(position_), position_, GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW); 
    set_layout( "position", 3, 5, 0 );
    set_layout( "texcoord", 2, 5, 3 );
    program_->uniform_block("Circle" )["InnerColor"] = glm::vec3(1.0f, 1.0f, 0.75f);
    program_->uniform_block("Circle" )["OuterColor"] = glm::vec3(0.392f, 0.706f, 0.983f);
    program_->uniform_block("Circle" )["InnerRadius"] = 0.25f;
    program_->uniform_block("Circle" )["OuterRadius"] = 0.45f;
    program_->uniform_block("Circle").copy();
}

void sol::f_draw( double currentTime )
{
    glUseProgram( *program_ );
    set_attribute( "Offset", offset_ );
 
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}
