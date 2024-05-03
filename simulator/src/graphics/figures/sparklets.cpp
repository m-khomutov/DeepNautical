/* 
 * File:   sparklets.cpp
 * Author: mkh
 * 
 * Created on 21 августа 2023 г., 14:49
 */

#include "sparklets.h"

TSparklets::TSparklets( const std::vector< std::string > &settings, const glm::vec3 &camera_pos )
: TFigure( settings, camera_pos )
{
    // проверить настройки
    f_check_environment();
    // настроить геометрию точек случайными координатами
    f_set_points();
    // настроить модель
    model_ = glm::rotate( 
                 glm::rotate( 
                     glm::translate( model_, glm::vec3( 0.0f, 0.0f, -0.25f ) ),
                     glm::radians( 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) ),
                 glm::radians( -75.0f ), glm::vec3(1.0f, 0.0f, 0.0f) );
}

void TSparklets::draw( size_t vbo_number )
{
    set_uniform( "Time", GLfloat(last_frame_time_) );

    // точки и подложка рисуются через разные VBO
    switch( vbo_number ) {
    case 0:
    {
        f_draw_layout(); // подложжка
    } break;
    case 1:
    {
        f_draw_sparklets(); // точки - блики
    } break;
    }
}

void TSparklets::f_check_environment() const
{

    if( ! (NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["textures"]) + "/"     + spec_.texture_name).c_str()) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["textures"]) + "/"     + spec_.texture_air).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["textures"]) + "/"     + spec_.alpha).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + " " + spec_.texture_name + "}"  );
    }
}

char const *TSparklets::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void TSparklets::f_initialize( size_t vbo_number )
{
    switch( vbo_number ) {
    case 0:
    {
        f_initialize_layout();
    } break;
    case 1:
    {
        f_initialize_sparklets();
    } break;
    }
}

void TSparklets::f_accept( size_t vbo_number, IVisitor &p, double )
{
    last_frame_time_ += spec_.speed.x;
    p.visit( vbo_number, this );
}

void TSparklets::f_initialize_layout()
{
    // создать текстуру
    std::string alpha = std::string(NUtils::TConfig()["textures"]) + "/" + spec_.alpha;
    texture_.reset( new TJpegTexture( (std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_name).c_str(), alpha.c_str() ) );
    air_texture_.reset( new TJpegTexture( (std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_air).c_str() ) );

    // скопировать в шейдер индексы подложжки
    glBufferData( GL_ARRAY_BUFFER, spec_.viewport.size() * sizeof(float), spec_.viewport.data(), GL_STATIC_DRAW );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW); 
    // настроить атрибуты
    try
    {
        set_attribute( "WavePosition", 3, 5, 0 );
        set_attribute( "Texcoord", 2, 5, 3 );
    }
    catch( const std::runtime_error &e )
    {
        std::cerr << "sparklets error: " << e.what() << std::endl;
    }   
}

void TSparklets::f_initialize_sparklets()
{
    // скопитьвать точки в шейдер
    size_t whole_size = (points_.size() + colors_.size()) * sizeof(GLfloat);
    glBufferData(GL_ARRAY_BUFFER, whole_size, NULL, GL_STREAM_DRAW);
    
    off_t offset = 0;
    glBufferSubData( GL_ARRAY_BUFFER, offset, points_.size() * sizeof(GLfloat), points_.data() );
    offset += points_.size() * sizeof(GLfloat);
    glBufferSubData( GL_ARRAY_BUFFER, offset, colors_.size() * sizeof(GLfloat), colors_.data() );

    // настроить атрибуты
    try {
        offset = 0;
        set_attribute( "SparklePosition", 3, 3, offset );
        offset += points_.size();
        set_attribute( "SparkleColor", 3, 3, offset );
        offset += colors_.size();
        set_attribute( "SparkleStart", 3, 3, offset );
    }
    catch( const std::exception & e ) {
        std::cerr << e.what() << std::endl;
    }
}

void TSparklets::f_draw_layout()
{
    set_uniform( "DrawSparkles", 0.0f );
    set_uniform( "Surge", GLsizei(2), spec_.surge );
    set_uniform( "AirTexture", GLuint(1) );
    air_texture_->activate( GL_TEXTURE1 );
    
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

void TSparklets::f_draw_sparklets()
{
    set_uniform( "DrawSparkles", 1.0f );
    
    glDrawArrays( GL_POINTS, 0, points_.size() / 3 );
    f_reset_points();
}

void TSparklets::f_set_points()
{
    // случайные геометрические положения точек-бликов и их цветов
    for ( float x = spec_.viewport[0]; x < spec_.viewport[5]; x += spec_.step ) {
        float z { spec_.viewport[2] };
        for ( float y = spec_.viewport[11]; y < spec_.viewport[1]; y += spec_.step, z += spec_.step ) {;
            points_.push_back(((float)rand() / RAND_MAX) * 0.1 + x);
            points_.push_back(((float)rand() / RAND_MAX) * 0.1 + y);
            points_.push_back(z);

            colors_.push_back(((float)rand() / RAND_MAX) + 0.5);
            colors_.push_back(((float)rand() / RAND_MAX) + 0.5);
            colors_.push_back(((float)rand() / RAND_MAX) + 0.5);
        }
    }
}

void TSparklets::f_reset_points()
{
    float *pptr = points_.data();
    float *cptr = colors_.data();
    // случайные геометрические положения точек-бликов и их цветов
    for ( float x = spec_.viewport[0]; x < spec_.viewport[5]; x += spec_.step ) {
        for ( float y = spec_.viewport[11]; y < spec_.viewport[1]; y += spec_.step ) {
            *pptr++ = ((float)rand() / RAND_MAX) * 0.1 + x;
            *pptr++ = ((float)rand() / RAND_MAX) * 0.1 + y;
             pptr++;
            
            *cptr++ = ((float)rand() / RAND_MAX) + 0.5;
            *cptr++ = ((float)rand() / RAND_MAX) + 0.5;
            *cptr++ = ((float)rand() / RAND_MAX) + 0.5;
        }
    }
    // скопировать в шейдер
    off_t offset = 0;
    glBufferSubData( GL_ARRAY_BUFFER, offset, points_.size() * sizeof(GLfloat), points_.data() );
    offset += points_.size() * sizeof(GLfloat);
    glBufferSubData( GL_ARRAY_BUFFER, offset, colors_.size() * sizeof(GLfloat), colors_.data() );
}
