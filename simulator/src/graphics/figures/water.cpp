/* 
 * File:   water.cpp
 * Author: mkh
 * 
 * Created on 1 февраля 2023 г., 14:18
 */

#include "water.h"
#include <glm/gtc/noise.hpp>

namespace 
{
GLfloat waveGen( GLfloat x, GLfloat z, GLfloat t, const glm::vec2 &params )
{
    const GLfloat x2 = x - 3;
    const GLfloat z2 = z + 1;
    const GLfloat xx = x2 * x2;
    const GLfloat zz = z2 * z2;
  
    return (( 2 * (z + 1) * sinf (params[0] * sqrtf (xx + zz) - params[1] * t) + glm::perlin(glm::vec3(10 * x, 10 * z, t))) / 250);
}

GLfloat wakeGen( GLfloat x, GLfloat z, GLfloat t, const glm::vec2 &params )
{
    const GLfloat x2 = x - 3;
    const GLfloat xx = x2 * x2;
    const GLfloat zz = z * z;
    
    return (sinf ( params[0] * sqrt( xx + zz ) - params[1] * t ) + glm::perlin(glm::vec3(10 * x, 10 * z, t)) ) / 50;
}
}  // namespace

water::water( const std::vector< std::string > &settings )
: figure( settings )
{
    model_ = glm::rotate( 
                 glm::rotate( 
                     glm::translate( model_, glm::vec3( 0.0f, -0.1f, 0.0f ) ),
                     glm::radians( 15.0f ), glm::vec3(0.0f, 1.0f, 0.0f) ),
                 glm::radians( 10.0f ), glm::vec3(1.0f, 0.0f, 0.0f) );
}

water::~water()
{
}

void water::draw()
{
    //glEnable(GL_BLEND);  
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    //glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    set_attribute( "Offset", offset_ );
    set_attribute( "LightPosition", spec_.light_position );
    set_attribute( "LightColor", spec_.light_color );
    set_attribute( "FogParams.color", glm::vec4(0.392, 0.706, 0.983, 1.0f) );
    set_attribute( "FogParams.start", 4.0f );
    set_attribute( "FogParams.end", 2.5f);

    set_attribute( "AirTexture", GLuint(1) );
    air_texture_->activate( GL_TEXTURE1 );

    uint32_t len = 2 * (resolution + 1);
    for( uint32_t i(0); i < resolution; i++ )
    {
        glDrawArrays( GL_TRIANGLE_STRIP, i * len, len );
    }
    //glDisable(GL_BLEND);  
}

void water::set_wake_position( const std::vector< figure::position > &pos )
{
    wake_position_ = pos;   
}

void water::f_check_environment() const
{
    if( ! (utils::file_exists( (std::string(utils::config()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           utils::file_exists( (std::string(utils::config()["textures"]) + "/"     + spec_.texture_name).c_str())  &&
           utils::file_exists( (std::string(utils::config()["textures"]) + "/"     + spec_.texture_air).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + " " + spec_.texture_name + "}"  );
    }
}

char const *water::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void water::f_initialize()
{
    std::string alpha = spec_.alpha.empty() ? "" : std::string(utils::config()["textures"]) + "/" + spec_.alpha;
    texture_.reset( new texture( (std::string(utils::config()["textures"]) + "/" + spec_.texture_name).c_str(), alpha.empty() ? nullptr : alpha.c_str() ) );
    air_texture_.reset( new texture( (std::string(utils::config()["textures"]) + "/" + spec_.texture_air).c_str() ) );

    glBufferData( GL_ARRAY_BUFFER, sizeof(surface_) + sizeof(normals_), NULL, GL_STREAM_DRAW );
    
    set_layout( "position", 3, 3, 0 );
    set_layout( "normals", 3, 3, sizeof(surface_) / sizeof(GLfloat) );
}

void water::f_accept( visitor &p, double currentTime )
{
    f_load_surface( currentTime );

    p.visit( this );
}

void water::f_load_surface( double )
{
    phase_ += 0.01f;
    if( phase_ >= 360.0f )/*2 * glm::pi< GLfloat >() )*/ phase_ = 0.0f;
    
    const float delta = 2.7f / resolution;
    for( uint32_t z(0); z < resolution; ++z )
    {
        GLfloat z_coord = z * delta - 1.0f; 
        for( uint32_t x(0); x <= resolution; ++x )
        {
            GLfloat x_coord = x * delta - 1.0f;
            int idx = 6 * (x + z * (resolution + 1));
            surface_[idx + 3] = ( x_coord );
            surface_[idx + 4] = f_generate_surface( x_coord, z_coord );
            surface_[idx + 5] = z_coord;
            if( z )
            {
                int pidx = 6 * (x + (z - 1) * (resolution + 1));
                surface_[idx + 0] = surface_[pidx + 3];
                surface_[idx + 1] = surface_[pidx + 4];
                surface_[idx + 2] = surface_[pidx + 5];
            }
            else
            {
                surface_[idx + 0] = ( x_coord );
                surface_[idx + 1] = f_generate_surface( x_coord, z_coord );
                surface_[idx + 2] = z_coord;
            }

            // Normals
            glm::vec3 a = glm::vec3( surface_[idx + 3], surface_[idx + 4],  surface_[idx + 5] );
            glm::vec3 b = glm::vec3( surface_[idx + 3], surface_[idx + 1],  surface_[idx + 2] );
            glm::vec3 c = glm::vec3( surface_[idx + 9], surface_[idx + 10], surface_[idx + 5] );
            if( x >= resolution )
            {
                const float xn = (resolution + 1) * delta + 1;
                c = glm::vec3( xn, f_generate_surface( xn, a[2] ), a[2] );
            }
            glm::vec3 n = glm::normalize(glm::cross(c - a, b - a));
            normals_[idx + 3] = n[0];
            normals_[idx + 4] = n[1];
            normals_[idx + 5] = n[2];

            if( z )
            {
                int pidx = 6 * (x + (z - 1) * (resolution + 1));
                normals_[idx + 0] = normals_[pidx + 3];
                normals_[idx + 1] = normals_[pidx + 4];
                normals_[idx + 2] = normals_[pidx + 5];
            }
            else
            {
                a[1] = f_generate_surface( a[0], (z - 1) * delta - 1 );
                a[2] = (z - 1) * delta - 1;

                c[1] = f_generate_surface( c[0], b[2] );
                c[2]= b[2];

                n = glm::normalize(glm::cross(c - a, b - a));
                normals_[idx + 0] = n[0];
                normals_[idx + 1] = n[1];
                normals_[idx + 2] = n[2];
            }
        }
    }
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(surface_), surface_ );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(surface_), sizeof(normals_), normals_ );
}

GLfloat water::f_generate_surface(GLfloat x, GLfloat z)
{
    GLfloat rc = waveGen( x, z, phase_, spec_.wave );
    for( auto pos : wake_position_ )
    {
        if( pos.course.x ) {
            bool coords_inverted = pos.course.x < 0;
            bool in_x_trace = coords_inverted ? ( x < pos.current.x ) : x > -pos.current.x;
            bool in_z_trace = ( z > pos.current.z - 0.3f && z < pos.current.z);
            if( in_x_trace && in_z_trace )
                rc += wakeGen( x, z, coords_inverted ? phase_ : -phase_, spec_.wake );
        }
    }
    return rc;
}
