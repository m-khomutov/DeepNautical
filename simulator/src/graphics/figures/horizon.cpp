/* 
 * File:   horizon.cpp
 * Author: mkh
 * 
 * Created on 13 февраля 2023 г., 17:58
 */

#include "horizon.h"

namespace
{
char const shader_name[] = "horizon.glsl";
char const texture_name[] = "/56118_Peaceful_ocean_background_HD_BG.avi";
}

bool horizon::environment_valid()
{
    return utils::file_exists( (std::string(utils::config()["shaders"]) + "vert_" + shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["shaders"]) + "frag_" + shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["textures"]) + texture_name).c_str() );
}

horizon::horizon()
: avi_( (std::string(utils::config()["textures"]) + texture_name).c_str() )
{
}

horizon::~horizon()
{
}

char const *horizon::f_shader_name() const
{
    return shader_name; 
}

void horizon::f_initialize()
{
    texture_.reset( new texture( avi_.next_image() ) );

    glBufferData( GL_ARRAY_BUFFER, sizeof(position_), position_, GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW); 
    GLuint index = program_->attribute_index( "position" );
    glVertexAttribPointer( index, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0 );
    glEnableVertexAttribArray( index );
    index = program_->attribute_index( "texcoord" );
    glVertexAttribPointer( index, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray( index );   
}

void horizon::f_draw( double current_time )
{
    if( last_frame_time_ < 0.01f )
    {
        last_frame_time_ = current_time;
    }
    if( current_time - last_frame_time_ >= avi_.frame_duration() )
    {
        *texture_ = avi_.next_image();
        last_frame_time_ = current_time;
    }
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}