/* 
 * File:   vessel.cpp
 * Author: mkh
 * 
 * Created on 16 февраля 2023 г., 13:11
 */

#include "vessel.h"

vessel::vessel( const std::vector< std::string > &settings )
: figure( settings )
{
    f_check_environment();
    f_reset();
    f_set_model();
    
    object_.reset( new blender::object( (std::string(utils::config()["objs"]) + "/" + spec_.obj_name).c_str() ) );
    object_->load_position( &position_ );
}

vessel::~vessel()
{
}

void vessel::f_check_environment() const
{
    if( ! (utils::file_exists( (std::string(utils::config()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["objs"]) + "/" + spec_.obj_name).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + " " + spec_.obj_name + "}"  );
    }
}

char const *vessel::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void vessel::f_initialize()
{
    glBufferData( GL_ARRAY_BUFFER, position_.size() * sizeof(GLfloat), position_.data(), GL_STATIC_DRAW );
    try
    {
        set_layout( "position", 3, 8, 0 );
        set_layout( "texcoord", 2, 8, 3 );
        set_layout( "normal", 3, 8, 5 );
    }
    catch( const std::runtime_error &e )
    {
        std::cerr << "vessel error: " << e.what() << std::endl;
    }
}

void vessel::f_draw( double )
{
    f_set_model();
    try
    {
        set_attribute( "NormalMatrix", glm::transpose( glm::inverse( glm::mat3(view_ * model_) ) ) );
    }
    catch( const std::runtime_error &e )
    {
        std::cerr << "NormalMatrix error: " << e.what() << std::endl;
    }
    GLuint first = 0;
    for( auto mtl : object_->materials() )
    {
        try
        {
            program_->uniform_block("Material" )["Ka"] = mtl.Ka;
            program_->uniform_block("Material" )["Kd"] = mtl.Kd;
            program_->uniform_block("Material" )["Ks"] = mtl.Ks;
            program_->uniform_block("Material" )["Ns"] = mtl.Ns;
            program_->uniform_block("Material" )["Ni"] = mtl.Ni;
            program_->uniform_block("Material" )["d"] = mtl.d;
            program_->uniform_block("Material" )["illum"] = mtl.illum;
            program_->uniform_block("Material").copy();
        }
        catch( const std::runtime_error &e )
        {
            std::cerr << "Material error: " << e.what() << std::endl;
        }
        if( mtl.map_Kd )
        {
            try
            {
                mtl.map_Kd->activate();
                set_attribute( "Texture", GLuint(0) );
            }
            catch( const std::runtime_error &e )
            {
                std::cerr << "Texture error: " << e.what() << std::endl;
            }
        }
        glDrawArrays( GL_TRIANGLES, first, mtl.faces.size() * 3 );
        first += mtl.faces.size() * 3;
    }
}

void vessel::f_set_model()
{
    pitching_angle_ += spec_.pitching;
    if( pitching_angle_ < spec_.pitching_range[0] || pitching_angle_ > spec_.pitching_range[1] )
    {
        spec_.pitching *= -1;
    }
    model_ = glm::mat4( glm::scale(
                            glm::translate(
                                glm::rotate(
                                    glm::rotate(
                                        glm::rotate( glm::mat4(1.0f),
                                                     glm::radians( spec_.course.z + angle_.z ),
                                                     glm::vec3( 0.0f, 0.0f, 1.0f ) ),
                                        glm::radians( spec_.course.y + angle_.y ),
                                        glm::vec3( 0.0f, 1.0f, 0.0f ) ),
                                    glm::radians( spec_.course.x + angle_.x ),
                                    glm::vec3( 1.0f, 0.0f, 0.0f ) ),
                                offset_ ),
                            factor_ ) );
    if( ! f_full_trajectory() )
    {
        offset_ += spec_.speed;
        factor_ += spec_.factor_gain;
    }
    else
    {
        f_reset();
    }
    angle_ += spec_.angle_gain;
    angle_.x += spec_.pitching;
}

bool vessel::f_full_trajectory()
{
    if( spec_.speed.x < 0.0f )
    {
        return spec_.start_position.x - offset_.x >= spec_.trajectory;
    }
    return offset_.x - spec_.start_position.x >= spec_.trajectory;
}

void vessel::f_reset()
{
    offset_ = spec_.start_position; 
    factor_ = spec_.start_factor;
    angle_ = glm::vec3( 0.0f, 0.0f, 0.0f );
    pitching_angle_ = spec_.pitching_range[0];
    angle_.x = pitching_angle_;
}
