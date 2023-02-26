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
: objreader_( (std::string(utils::config()["objs"]) + obj_name).c_str() )
{
    speed_ = glm::vec3( 0.00035f, -0.00015f, 0.0f );
    offset_ = initial_offset; 
    angle_ = lurch_range[0];
    
    objreader_.load_position( &position_ );
    
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
    set_attribute( "NormalMatrix", glm::transpose( glm::inverse( glm::mat3(view_ * model_) ) ) );
    GLuint first = 0;
    for( auto mtl : objreader_.materials() )
    {

        program_->uniform_block("Material" )["Ka"] = mtl.Ka;
        program_->uniform_block("Material" )["Kd"] = mtl.Kd;
        program_->uniform_block("Material" )["Ks"] = mtl.Ks;
        program_->uniform_block("Material" )["Ns"] = mtl.Ns;
        program_->uniform_block("Material" )["Ni"] = mtl.Ni;
        program_->uniform_block("Material" )["d"] = mtl.d;
        program_->uniform_block("Material" )["illum"] = mtl.illum;
        program_->uniform_block("Material").copy();
        glDrawArrays( GL_TRIANGLES, first, mtl.faces.size() * 3 );
        first += mtl.faces.size() * 3;
    }
}

void vessel::f_set_model()
{
    if( angle_ < lurch_range[0] || angle_ > lurch_range[1] )
    {
        lurch_ *= -1;
    }
    model_ = glm::mat4( glm::scale(
                            glm::translate(
                                glm::rotate( glm::mat4(1.0f),
                                             glm::radians( angle_ - 90.0f ),
                                             glm::vec3( 1.0f, 0.0f, 0.0f ) ),
                                offset_ ),
                            factor_ ) );
    if( offset_.x < 1.42f )
    {
        offset_ += speed_;
        factor_ += factor_offset;
    }
    else
    {
        offset_ = initial_offset; 
        factor_ = initial_factor;
    }
    angle_ += lurch_;
}
