/* 
 * File:   figure.cpp
 * Author: mkh
 * 
 * Created on 31 января 2023 г., 14:35
 */

#include "figures/figure.h"

#include <QOpenGLContext>

TFigure::TFigure( const NJson::TObject &environment,const NJson::TObject &settings )
: spec_( environment, settings )
{
    // настроить обзор и перспективу
    NUtils::TGeometry win { NUtils::TConfig()["window"] };

    projection_.setToIdentity();
    projection_.perspective( 60.0, (float) win.width / (float) win.height, 0.001, 1000 );

    view_.setToIdentity();
    view_.lookAt( spec_.camera_position,
                  QVector3D(0.0f, 0.0f, 0.0f),
                  QVector3D(0.0f, 1.0f, 0.0f) );

    model_.setToIdentity();
    model_.rotate( 0.0f, QVector3D(1.0f, 0.0f, 0.0f) );
}

TFigure::~TFigure()
{
    texture_.reset();
}

void TFigure::initialize()
{
    std::string dir = NUtils::TConfig()["shaders"];
    NUtils::read_directory( dir, f_shader_name(), [&]( const std::string &name ){
        if( name.find( "vert_" ) == 0 )
        {
            shader_program_.addShaderFromSourceFile( QGLShader::Vertex, std::string(dir + "/" + name).c_str() );
        }
        else if( name.find( "frag_" ) == 0 )
        {
            shader_program_.addShaderFromSourceFile( QGLShader::Fragment, std::string(dir + "/" + name).c_str() );
        }
    } );
    shader_program_.link();

    shader_program_.bind();

    f_initialize();

    shader_program_.release();
    valid_ = true;
}

void TFigure::accept( IVisitor &p, double currentTime )
{
    shader_program_.bind();

    // настроить униформные переменные
    shader_program_.setUniformValue( "Model", model_ );
    shader_program_.setUniformValue( "View", view_ );
    shader_program_.setUniformValue( "Projection", projection_ );
    if( texture_ )
    {
        shader_program_.setUniformValue( "Texture", GLuint(0) );
        texture_->bind(0);
    }
    // в объекте производного класса передать посетителю указатель на себя
    f_accept( p, currentTime );

    shader_program_.release();
}
