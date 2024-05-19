/* 
 * File:   sparklets.cpp
 * Author: mkh
 * 
 * Created on 21 августа 2023 г., 14:49
 */

#include "figures/surge.h"

TSurge::TSurge( const NJson::TObject &environment,const NJson::TObject &settings )
: TFigure( environment, settings )
{
    // проверить настройки
    f_check_environment();
    // настроить геометрию точек случайными координатами
    f_set_points();
    // настроить модель
    model_.setToIdentity();
    model_.translate( QVector3D(0.f, 0.f, -1.0f) );
    model_.rotate( -25.f, QVector3D(1.0f, 0.0f, 0.0) );

    float *ptr = spec_.viewport.data();
    for( size_t i(0); i < spec_.viewport.size(); i += 5 )
    {
        layout_ << QVector4D(ptr[i], ptr[i+1], ptr[i+2], 1.f);
        texels_ << QVector2D(ptr[i+3], ptr[i+4]);
    }
}

void TSurge::draw()
{
    shader_program_.setUniformValue( "Time", last_frame_time_ );

    f_draw_layout();    // подложка
    f_draw_sparklets(); // точки - блики
}

void TSurge::f_check_environment() const
{

    if( ! (NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["textures"]) + "/"     + spec_.texture_name).c_str()) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["textures"]) + "/"     + spec_.texture_air).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["textures"]) + "/"     + spec_.texture_alpha).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + " " + spec_.texture_name + "}"  );
    }
}

char const *TSurge::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void TSurge::f_initialize()
{
    f_initialize_layout();
}

void TSurge::f_accept( IVisitor &p, double )
{
    last_frame_time_ += spec_.speed.x();
    p.visit( this );
}

void TSurge::f_initialize_layout()
{
    // создать текстуру
    //std::string alpha = std::string(NUtils::TConfig()["textures"]) + "/" + spec_.alpha;
    texture_.reset( new QOpenGLTexture( QImage(std::string(std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_name).c_str() ) ) );
    air_texture_.reset( new QOpenGLTexture( QImage(std::string(std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_air).c_str() ) ) );
}

void TSurge::f_draw_layout()
{
    try
    {
        shader_program_.setAttributeArray( "WavePosition", layout_.constData() );
        shader_program_.enableAttributeArray( "WavePosition" );
        shader_program_.setAttributeArray( "Texcoord", texels_.constData() );
        shader_program_.enableAttributeArray( "Texcoord" );
    }
    catch( const std::runtime_error &e )
    {
        qDebug() << "sparklets error: " << e.what();
        return;
    }
    // настроить переменную положения в шейдере
    shader_program_.setUniformValue( "DrawSparkles", 0.f );
    shader_program_.setUniformValue( "FogParams.color", spec_.fog_color );
    shader_program_.setUniformValue( "FogParams.density", spec_.fog_density );
    shader_program_.setUniformValue( "Amplitude", spec_.ripple[0] );
    shader_program_.setUniformValue( "Frequency", spec_.ripple[1] );
    if( air_texture_ )
    {
        shader_program_.setUniformValue( "AirTexture", GLuint(1) );
        air_texture_->bind(1);
    }
    // отрисовать объект
    glDrawArrays( GL_TRIANGLE_FAN, 0, layout_.size() );

    shader_program_.disableAttributeArray("Texcoord");
    shader_program_.disableAttributeArray("WavePosition");
}

void TSurge::f_draw_sparklets()
{
    shader_program_.setAttributeArray( "SparklePosition", points_.data(), 3 );
    shader_program_.enableAttributeArray( "SparklePosition" );
    shader_program_.setAttributeArray( "SparkleColor", colors_.data(), 3 );
    shader_program_.enableAttributeArray( "SparkleColor" );

    shader_program_.setUniformValue( "DrawSparkles", 1.f );

    glDrawArrays( GL_POINTS, 0, points_.size() / 3 );

    shader_program_.disableAttributeArray("SparklePosition");
    shader_program_.disableAttributeArray("SparkleColor");

    f_reset_points();
}

void TSurge::f_set_points()
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

void TSurge::f_reset_points()
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
}
