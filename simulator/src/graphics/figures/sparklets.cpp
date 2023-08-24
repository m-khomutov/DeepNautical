/* 
 * File:   sparklets.cpp
 * Author: mkh
 * 
 * Created on 21 августа 2023 г., 14:49
 */

#include "sparklets.h"

sparklets::sparklets( const std::vector< std::string > &settings )
: figure( settings )
{
    f_check_environment();
    model_ = glm::rotate( 
                 glm::rotate( 
                     glm::translate( model_, glm::vec3( 0.0f, 0.0f, -0.25f ) ),
                     glm::radians( 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) ),
                 glm::radians( -75.0f ), glm::vec3(1.0f, 0.0f, 0.0f) );
}

sparklets::~sparklets()
{
}

void sparklets::draw( size_t )
{
    set_uniform( "Surge", GLsizei(2), spec_.surge );
    set_uniform( "Time", GLfloat(last_frame_time_) );
    set_uniform( "AirTexture", GLuint(1) );
    air_texture_->activate( GL_TEXTURE1 );
    
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

void sparklets::f_check_environment() const
{

    if( ! (utils::file_exists( (std::string(utils::config()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["textures"]) + "/"     + spec_.texture_name).c_str()) &&
           utils::file_exists( (std::string(utils::config()["textures"]) + "/"     + spec_.texture_air).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["textures"]) + "/"     + spec_.alpha).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + " " + spec_.texture_name + "}"  );
    }
}

char const *sparklets::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void sparklets::f_initialize( size_t )
{
    std::string alpha = std::string(utils::config()["textures"]) + "/" + spec_.alpha;
    texture_.reset( new texture( (std::string(utils::config()["textures"]) + "/" + spec_.texture_name).c_str(), alpha.c_str() ) );
    air_texture_.reset( new texture( (std::string(utils::config()["textures"]) + "/" + spec_.texture_air).c_str() ) );

    glBufferData( GL_ARRAY_BUFFER, spec_.viewport.size() * sizeof(float), spec_.viewport.data(), GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW); 
    try
    {
        set_attribute( "Position", 3, 5, 0 );
        set_attribute( "Texcoord", 2, 5, 3 );
    }
    catch( const std::runtime_error &e )
    {
        std::cerr << "sparklets error: " << e.what() << std::endl;
    }
}

void sparklets::f_accept( size_t vbo_number, visitor &p, double )
{
    last_frame_time_ += spec_.speed.x;
    p.visit( vbo_number, this );
}

void sparklets::f_set_points()
{
    const float delta = 2.7f / resolution;
    for( uint32_t z(0); z < resolution; ++z )
    {
        GLfloat z_coord = z * delta - 1.0f; 
        for( uint32_t x(0); x <= resolution; ++x )
        {
            GLfloat x_coord = x * delta - 1.0f;
            points_.push_back( ((float)::rand() / RAND_MAX) * 0.1 + x_coord );    
            points_.push_back( 0.0f );    
            points_.push_back( ((float)::rand() / RAND_MAX) * 0.1 + z_coord );    
        }
    }
}

void sparklets::f_reset_points()
{
    auto iter = points_.begin();
    const float delta = 2.7f / resolution;
    for( uint32_t z(0); z < resolution; ++z )
    {
        GLfloat z_coord = z * delta - 1.0f; 
        for( uint32_t x(0); x <= resolution; ++x )
        {
            GLfloat x_coord = x * delta - 1.0f;
            *iter++ = ((float)::rand() / RAND_MAX) * 0.1 + x_coord;    
             iter++;    
            *iter++ = ((float)::rand() / RAND_MAX) * 0.1 + z_coord;    
        }
    }
    glBufferSubData( GL_ARRAY_BUFFER, 0, points_.size() * sizeof(GLfloat), points_.data() );
}
