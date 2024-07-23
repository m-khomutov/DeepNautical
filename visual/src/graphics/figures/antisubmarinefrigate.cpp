/* 
 * File:   antisubmarinefrigate.cpp
 * Author: mkh
 * 
 * Created on 31 января 2023 г., 14:45
 */

#include "antisubmarinefrigate.h"

TAntisubmarinefrigate::TAntisubmarinefrigate( const std::vector< std::string > &settings, const glm::vec3 &camera_pos )
: TFigure( settings, camera_pos )
{
    f_set_model();
}

void TAntisubmarinefrigate::draw( size_t )
{
    // передаем униформные переменные
    set_uniform( "Texture", GLuint(0) );
    set_uniform( "Offset", offset_ );
    set_uniform( "Waterline", waterline_ );
    // настраиваем местоположение на сцене
    if( scene_position_ < -1.5 || scene_position_ > 1.5 )
    {
        offset_.x = 0.0f;
        direction_ *= -1;
        angle_.x = angle_.x == 0.0f ? 180.0f : 0.0f;
        f_set_model();
    }
    scene_position_ += spec_.speed.x * direction_;
    offset_.x -= spec_.speed.x;

    // рисуем
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );   
}

void TAntisubmarinefrigate::f_check_environment() const
{
    // проверяем наличие файлов, указанных в конфигурации
    if( ! (utils::exists( (std::string(utils::settings()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           utils::exists( (std::string(utils::settings()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           utils::exists( (std::string(utils::settings()["textures"]) + "/" + spec_.texture_name).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + " " + spec_.texture_name + "}"  );
    }
}

char const *TAntisubmarinefrigate::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void TAntisubmarinefrigate::f_initialize( size_t )
{
    // создаем текстуру
    texture_.reset( new TJpegTexture( (std::string(utils::settings()["textures"]) + spec_.texture_name).c_str() ) );

    // ыделяем память под данные
    glBufferData( GL_ARRAY_BUFFER, sizeof(position_), position_, GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW); 
    // настраиваем атрибуты
    set_attribute( "Position", 3, 5, 0 );
    set_attribute( "Texcoord", 2, 5, 3 );
}

void TAntisubmarinefrigate::f_accept( size_t vbo_number, IVisitor &p, double )
{
    // передаем посетителю указатель на себя. Запустит нашу отрисовку когда надо
    p.visit( vbo_number, this );
}

void TAntisubmarinefrigate::f_set_model()
{
    // местоположение на сцене в пространстве
    GLfloat y = angle_.x == 0.0f ? -0.08f : -0.38f;
    glm::vec3 factor = angle_.x == 0.0f ? glm::vec3( 0.7f ) : glm::vec3( 1.0f );
    model_ = glm::mat4( glm::scale(
                            glm::translate(
                                glm::rotate( glm::mat4(1.0f),
                                             glm::radians( angle_.x ),
                                             glm::vec3( 0.0f, 1.0f, 0.09f ) ),
                                glm::vec3( 1.0f, y, 0.0f ) ),
                            factor ) );
}
