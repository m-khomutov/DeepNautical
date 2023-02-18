/* 
 * File:   vessel.cpp
 * Author: mkh
 * 
 * Created on 16 февраля 2023 г., 13:11
 */

#include "vessel.h"

namespace
{
char const shader_name[] = "vessel.glsl";
char const obj_name[] = "/vessel.obj";
}

bool vessel::environment_valid()
{
    return utils::file_exists( (std::string(utils::config()["shaders"]) + "/vert_" + shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["shaders"]) + "/frag_" + shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["objs"]) + obj_name).c_str() );
}

vessel::vessel()
{
    objreader((std::string(utils::config()["objs"]) + obj_name).c_str()).load_position( &position_ );
    f_set_model();
}

vessel::~vessel()
{
}

char const *vessel::f_shader_name() const
{
    return shader_name; 
}

void vessel::f_initialize()
{
    glBufferData( GL_ARRAY_BUFFER, position_.size() * sizeof(GLfloat), position_.data(), GL_STATIC_DRAW );
    set_layout( "position", 3, 8, 0 );
    set_layout( "texcoord", 2, 8, 3 );
    set_layout( "normal", 3, 8, 5 );
}

void vessel::f_draw( double currentTime )
{
    f_set_model();
    GLuint count = position_.size() / 3;
    glDrawArrays( GL_TRIANGLES, 0, count );
}

void vessel::f_set_model()
{
    GLfloat y = 0.0f;
    glm::vec3 factor = glm::vec3( 1.0f );
    model_ = glm::mat4( glm::scale(
                            glm::translate(
                                glm::rotate( glm::mat4(1.0f),
                                             glm::radians( angle_ ),
                                             glm::vec3( 0.0f, 0.0f, 0.0f ) ),
                                glm::vec3( 0.0f, 0.0f, 0.0f ) ),
                            factor ) );
    model_ = glm::mat4(1.0f);
}
