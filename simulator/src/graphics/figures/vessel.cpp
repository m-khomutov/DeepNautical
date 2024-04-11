/* 
 * File:   vessel.cpp
 * Author: mkh
 * 
 * Created on 16 февраля 2023 г., 13:11
 */

#include "vessel.h"

vessel::vessel( const std::vector< std::string > &settings, const glm::vec3 &camera_pos )
: figure( settings, camera_pos )
{
    f_check_environment();
    
    object_.reset( new blender::object( (std::string(NUtils::config()["objs"]) + "/" + spec_.obj_name).c_str() ) );
    object_->load_position( &vertices_ );
    
    f_reset();
}

vessel::~vessel()
{
}

void vessel::draw( size_t )
{
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    set_uniform( "NormalMatrix", glm::transpose( glm::inverse( glm::mat3(view_ * model_) ) ) );
    set_uniform( "LightPosition", spec_.light_position );
    set_uniform( "LightColor", spec_.light_color );
    set_uniform( "CameraPosition", spec_.camera_position );
    set_uniform( "fog.color", spec_.fog_color );
    set_uniform( "fog.density", spec_.fog_density );

    GLuint first = 0;
    for( auto mtl : object_->materials() )
    {
        try
        {
            program_->uniform_block("Material" )["Ka"].set( mtl.Ka );
            program_->uniform_block("Material" )["Kd"].set( mtl.Kd );
            program_->uniform_block("Material" )["Ks"].set( mtl.Ks );
            program_->uniform_block("Material" )["Ns"].set( mtl.Ns );
            program_->uniform_block("Material" )["Ni"].set( mtl.Ni );
            program_->uniform_block("Material" )["d"].set( mtl.d );
            program_->uniform_block("Material" )["illum"].set( mtl.illum );
            program_->uniform_block("Material").copy();
        }
        catch( const std::runtime_error &e )
        {
            std::cerr << "Material error: " << e.what() << std::endl;
        }
        try
        {
            if( mtl.map_Kd )
            {
                mtl.map_Kd->activate();
            }
            else
            {
                empty_texture_->activate();
            }
            set_uniform( "Texture", GLuint(0) );
        }
        catch( const std::runtime_error &e )
        {
            std::cerr << "Texture error: " << e.what() << std::endl;
        }
        glDrawArrays( GL_TRIANGLES, first, mtl.faces.size() * 3 );
        first += mtl.faces.size() * 3;
    }
}

const figure::position &vessel::position()
{
    return position_;
}

void vessel::f_check_environment() const
{
    if( ! (NUtils::file_exists( (std::string(NUtils::config()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::config()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::config()["objs"]) + "/" + spec_.obj_name).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + " " + spec_.obj_name + "}"  );
    }
}

char const *vessel::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void vessel::f_initialize( size_t )
{
    empty_texture_.reset( new texture(1, 1, 255) );
    glBufferData( GL_ARRAY_BUFFER, vertices_.size() * sizeof(GLfloat), vertices_.data(), GL_STATIC_DRAW );
    try
    {
        set_attribute( "Position", 3, 8, 0 );
        set_attribute( "Texcoord", 2, 8, 3 );
        set_attribute( "Normal", 3, 8, 5 );
    }
    catch( const std::runtime_error &e )
    {
        std::cerr << "vessel error: " << e.what() << std::endl;
    }
}

void vessel::f_accept( size_t vbo_number, visitor &p, double )
{
    f_set_model();
    p.visit( vbo_number, this );
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
        position_.current.x +=  spec_.speed.x;
        position_.current.y +=  spec_.speed.y;
        position_.current.z += spec_.course.x < 0 ? -spec_.speed.z :  spec_.speed.z;
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
    
    f_set_model();

    position_.current = offset_;
    position_.course = spec_.course;
    position_.wake = spec_.wake_width;
}
