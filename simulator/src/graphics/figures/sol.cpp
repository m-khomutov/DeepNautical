/* 
 * File:   sol.cpp
 * Author: mkh
 * 
 * Created on 1 февраля 2023 г., 10:12
 */

#include "sol.h"
#include <glm/gtx/transform.hpp>

namespace
{
char const shader_name[] = "sol.glsl";
}
bool sol::environment_valid()
{
    return utils::file_exists( (std::string(utils::config()["shaders"]) + "/vert_" + shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["shaders"]) + "/frag_" + shader_name).c_str() );
}

sol::sol()
{
    offset_ = glm::vec3( -2.5f, 2.0f, 0.0f );
    model_ = glm::mat4( glm::scale( 
                            glm::translate( glm::vec3(0.0f, 0.0f, -1.0f) ),
                            glm::vec3( 0.3f ) ) );
}

sol::~sol()
{
}


char const *sol::f_shader_name() const
{
    return shader_name; 
}

void sol::f_initialize()
{
    glBufferData( GL_ARRAY_BUFFER, sizeof(position_), position_, GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW); 
    set_layout( "position", 3, 5, 0 );
    set_layout( "texcoord", 2, 5, 3 );
    program_->uniform_block("Circle" )["InnerColor"] = glm::vec4(1.0f, 1.0f, 0.75f, 1.0f);
    program_->uniform_block("Circle" )["OuterColor"] = glm::vec4(0.392f, 0.706f, 0.983f, 1.0f);
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
