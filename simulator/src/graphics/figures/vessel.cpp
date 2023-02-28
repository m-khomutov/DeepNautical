/* 
 * File:   vessel.cpp
 * Author: mkh
 * 
 * Created on 16 февраля 2023 г., 13:11
 */

#include "vessel.h"

vessel::vessel( const std::vector< std::string > &settings )
{
    f_parse_settings( settings );
    f_check_environment();
    
    objreader_.reset( new objreader( (std::string(utils::config()["objs"]) + "/" + obj_name_).c_str() ) );

    offset_ = start_position_; 
    factor_ = start_factor_;
    angle_ = pitching_[0];
    
    objreader_->load_position( &position_ );
    
    f_set_model();
}

vessel::~vessel()
{
}


void vessel::f_parse_settings( const std::vector< std::string > &settings )
{
    for( auto s : settings )
    {
        std::pair< std::string, std::string > p;
        if( utils::str2key( s, &p ) )
        {
            if( p.first.find( "shader" ) != std::string::npos )
            {
                shader_name_ = p.second.substr( 1, p.second.size() - 2 );
            }
            else if( p.first.find( "object" ) != std::string::npos )
            {
                obj_name_ = p.second.substr( 1, p.second.size() - 2 );
            }
            else if( p.first.find( "speed" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &speed_ ) )
                {
                    std::cerr << "vessel error: invalid speed\n";
                }
            }
            else if( p.first.find( "pitching_angles" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &pitching_ ) )
                {
                    std::cerr << "vessel error: invalid pitching\n";
                }
            }
            else if( p.first.find( "start_position" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &start_position_ ) )
                {
                    std::cerr << "vessel error: invalid start position\n";
                }
            }
            else if( p.first.find( "start_factor" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &start_factor_ ) )
                {
                    std::cerr << "vessel error: invalid start factor\n";
                }
            }
            else if( p.first.find( "factor_gain" ) != std::string::npos )
            {
                factor_gain_ = std::stof( p.second );
            }
            else if( p.first.find( "pitching_gain" ) != std::string::npos )
            {
                pitching_gain_ = std::stof( p.second );
            }
            else if( p.first.find( "x_trajectory" ) != std::string::npos )
            {
                x_trajectory_ = std::stof( p.second );
            }
        }
    }
}

void vessel::f_check_environment() const
{
    if( ! (utils::file_exists( (std::string(utils::config()["shaders"]) + "/vert_" + shader_name_).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["shaders"]) + "/frag_" + shader_name_).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["objs"]) + "/" + obj_name_).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + shader_name_ + " " + obj_name_ + "}"  );
    }
}

char const *vessel::f_shader_name() const
{
    return shader_name_.c_str(); 
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

void vessel::f_draw( double currentTime )
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
    for( auto mtl : objreader_->materials() )
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
        glDrawArrays( GL_TRIANGLES, first, mtl.faces.size() * 3 );
        first += mtl.faces.size() * 3;
    }
}

void vessel::f_set_model()
{
    if( angle_ < pitching_[0] || angle_ > pitching_[1] )
    {
        pitching_gain_ *= -1;
    }
    model_ = glm::mat4( glm::scale(
                            glm::translate(
                                glm::rotate( glm::mat4(1.0f),
                                             glm::radians( angle_ - 90.0f ),
                                             glm::vec3( 1.0f, 0.0f, 0.0f ) ),
                                offset_ ),
                            factor_ ) );
    if( offset_.x < x_trajectory_ + start_position_.x )
    {
        offset_ += speed_;
        factor_ += factor_gain_;
    }
    else
    {
        offset_ = start_position_; 
        factor_ = start_factor_;
    }
    angle_ += pitching_gain_;
}
