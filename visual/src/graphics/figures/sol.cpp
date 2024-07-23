/* 
 * File:   sol.cpp
 * Author: mkh
 * 
 * Created on 1 февраля 2023 г., 10:12
 */

#include "sol.h"
#include <glm/gtx/transform.hpp>


TSol::TSol( const std::vector< std::string > &settings, const glm::vec3 &camera_pos )
: TFigure( settings, camera_pos )
{
    // настраиваем местоположение и геометрическую модель объекта
    offset_ = glm::vec3( -2.5f, 2.0f, 0.0f );
    model_ = glm::mat4( glm::scale( 
                            glm::rotate(
                                glm::translate( glm::vec3(0.0f, 0.0f, -1.0f) ),
                                glm::radians( 50.0f ), glm::vec3(1.0f, 0.0f, 0.0f) ),
                            glm::vec3( 0.3f ) ) );
}

void TSol::draw( size_t )
{
    // настроить переменную положения в шейдере
    set_uniform( "Offset", offset_ );
    // отрисовать объект
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

void TSol::f_check_environment() const
{
    if( ! (utils::exists( (std::string(utils::settings()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           utils::exists( (std::string(utils::settings()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           utils::exists( (std::string(utils::settings()["textures"]) + "/" + spec_.texture_name).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + "}"  );
    }
}

char const *TSol::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void TSol::f_initialize( size_t )
{
    // создать текстуру
    texture_.reset( new TJpegTexture( (std::string(utils::settings()["textures"]) + "/" + spec_.texture_name).c_str() ) );

    // выделить память под атрибуты в шейдере
    glBufferData( GL_ARRAY_BUFFER, spec_.viewport.size() * sizeof(float), spec_.viewport.data(), GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW);
    // настроить атрибуты
    set_attribute( "Position", 3, 5, 0 );
    set_attribute( "Texcoord", 2, 5, 3 );
    // выставить униформные переменные
    set_uniform( "CameraPosition", spec_.camera_position );
    set_uniform( "Fog.color", spec_.fog_color );
    set_uniform( "Fog.density", spec_.fog_density );

    program_->uniform_block("CircleParams")["OuterColor"].set( glm::vec3(0.392f, 0.706f, 0.983f) );
    program_->uniform_block("CircleParams")["InnerColor"].set( glm::vec3(1.0f, 1.0f, 0.75f) );
    program_->uniform_block("CircleParams")["OuterColor"].set( glm::vec3(0.392f, 0.706f, 0.983f) );
    program_->uniform_block("CircleParams")["InnerRadius"].set( 0.25f );
    program_->uniform_block("CircleParams")["OuterRadius"].set( 0.45f );
    program_->uniform_block("CircleParams").copy();
}

void TSol::f_accept( size_t vbo_number, IVisitor &p, double )
{
    // передать посетителю указатель на себя
    p.visit( vbo_number, this );
}
