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
}

TAir::~TAir()
{
    shader_program_.disableAttributeArray("Texcoord");
    shader_program_.disableAttributeArray("Position");
}

void TAir::draw()
{
    // настроить переменную положения в шейдере
    shader_program_.setUniformValue( "Offset", offset_ );
    shader_program_.setUniformValue( "Fog.color", spec_.fog_color );
    shader_program_.setUniformValue( "Fog.density", spec_.fog_density );
    shader_program_.setUniformValue( "Time", last_frame_time_ );
    // отрисовать объект
    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
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
    vertex_buffer_object_.allocate( spec_.viewport.data(), spec_.viewport.size() * sizeof(GLfloat) );

    shader_program_.enableAttributeArray( "Position" );
    shader_program_.setAttributeBuffer( "Position", GL_FLOAT, 0, 3, 5 * sizeof(GLfloat) );

    shader_program_.enableAttributeArray( "Texcoord" );
    shader_program_.setAttributeBuffer( "Texcoord", GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    // создать текстуру
    texture_.reset( new QOpenGLTexture( QImage(std::string(std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_name).c_str() ) ) );
}

void TAir::f_accept( IVisitor &p, double )
{
    last_frame_time_ += spec_.speed.x();
    // передать посетителю указатель на себя
    p.visit( this );
}
