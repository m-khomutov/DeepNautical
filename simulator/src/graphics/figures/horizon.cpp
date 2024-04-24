/* 
 * File:   horizon.cpp
 * Author: mkh
 * 
 * Created on 13 февраля 2023 г., 17:58
 */

#include "horizon.h"

THorizon::THorizon( const std::vector< std::string > &settings, const glm::vec3 &camera_pos )
: TFigure( settings, camera_pos )
{
    f_check_environment();  // проверить настройки
    // создать объект кадров формата AVI
    avi_.reset( new TAviTexture( (std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_name).c_str() ) );
}

void THorizon::draw( size_t )
{
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

void THorizon::f_check_environment() const
{
    if( ! (NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_name).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + " " + spec_.texture_name + "}"  );
    }
}

char const *THorizon::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void THorizon::f_initialize( size_t )
{
    // создать текстуру
    texture_.reset( new texture( avi_->next_image() ) );
    // выделить память под данные атрибутов в шейдере
    glBufferData( GL_ARRAY_BUFFER, spec_.viewport.size() * sizeof(float), spec_.viewport.data(), GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW);
    // выставить атрибуты
    set_attribute( "Position", 3, 5, 0 );
    set_attribute( "Texcoord", 2, 5, 3 );
}

void THorizon::f_accept( size_t vbo_number, visitor &p, double current_time )
{
    if( last_frame_time_ < 0.01f )
    {
        last_frame_time_ = current_time;
    }
    // если время кадра вышло, взять следующий кадр текстуры
    if( current_time - last_frame_time_ >= avi_->frame_duration() )
    {
        *texture_ = avi_->next_image();
        last_frame_time_ = current_time;
    }
    // передать указатель себя посетителю
    p.visit( vbo_number, this );
}
