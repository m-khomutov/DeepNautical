/* 
 * File:   sol.cpp
 * Author: mkh
 * 
 * Created on 1 февраля 2023 г., 10:12
 */

#include "figures/air.h"

TAir::TAir( const NJson::TObject &environment,const NJson::TObject &settings )
: TFigure( environment, settings )
{
    // настраиваем местоположение и геометрическую модель объекта
    offset_ = QVector4D( -2.5f, 2.0f, 0.0f, 1.f );

    model_.setToIdentity();
    model_.translate( QVector3D(0.0f, 0.0f, -1.0f) );
    model_.rotate( 50.f, QVector3D(1.0f, 0.0f, 0.0) );
    model_.scale( QVector3D(0.3f, 0.3f, 0.3f) );

    float *ptr = spec_.viewport.data();
    for( size_t i(0); i < spec_.viewport.size(); i += 5 )
    {
        vertices_ << QVector4D(ptr[i], ptr[i+1], ptr[i+2], 1.f);
        texels_ << QVector2D(ptr[i+3], ptr[i+4]);
    }
}

TAir::~TAir()
{
    shader_program_.disableAttributeArray("Texcoord");
    shader_program_.disableAttributeArray("Position");
}

void TAir::draw()
{
    shader_program_.setAttributeArray("Position", vertices_.constData());
    shader_program_.enableAttributeArray("Position");
    shader_program_.setAttributeArray("Texcoord", texels_.constData());
    shader_program_.enableAttributeArray("Texcoord");

    // настроить переменную положения в шейдере
    shader_program_.setUniformValue( "Offset", offset_ );
    shader_program_.setUniformValue( "Fog.color", spec_.fog_color );
    shader_program_.setUniformValue( "Fog.density", spec_.fog_density );
    shader_program_.setUniformValue( "Time", last_frame_time_ );
    // отрисовать объект
    glDrawArrays( GL_TRIANGLE_FAN, 0, vertices_.size() );

    shader_program_.disableAttributeArray("Texcoord");
    shader_program_.disableAttributeArray("Position");
}

void TAir::f_check_environment() const
{
    if( ! (NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_name).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + "}"  );
    }
}

char const *TAir::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void TAir::f_initialize()
{
    // создать текстуру
    texture_.reset( new QOpenGLTexture( QImage(std::string(std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_name).c_str() ) ) );
}

void TAir::f_accept( IVisitor &p, double )
{
    last_frame_time_ += spec_.speed.x();
    // передать посетителю указатель на себя
    p.visit( this );
}
