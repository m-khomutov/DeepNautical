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
, vertex_buffer_object_( QOpenGLBuffer(QOpenGLBuffer::VertexBuffer) )
{
    camera_view_.setToIdentity();
    camera_view_.lookAt( spec_.camera_position,
                         QVector3D(0.0f, 0.0f, 0.0f),
                         QVector3D(0.0f, 1.0f, 0.0f) );

    model_.setToIdentity();
    model_.rotate( 0.0f, QVector3D(1.0f, 0.0f, 0.0f) );
}

TFigure::~TFigure()
{
    texture_.reset();

    vertex_buffer_object_.destroy();
    vertex_array_object_.destroy();
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

    vertex_array_object_.create();
    vertex_array_object_.bind();

    vertex_buffer_object_.create();
    vertex_buffer_object_.bind();

    f_initialize();

    vertex_buffer_object_.release();
    vertex_array_object_.release();

    shader_program_.release();

    valid_ = true;
}

void TFigure::accept( IVisitor &p, double currentTime, const TCamera &camera )
{
    camera_view_ = camera.view();

    shader_program_.bind();
    vertex_array_object_.bind();
    vertex_buffer_object_.bind();

    // настроить униформные переменные
    shader_program_.setUniformValue( "Model", model_ );
    shader_program_.setUniformValue( "View", camera_view_ );
    shader_program_.setUniformValue( "Projection", camera.projection() );
    if( texture_ )
    {
        shader_program_.setUniformValue( "Texture", GLuint(0) );
        texture_->bind();
    }
    // в объекте производного класса передать посетителю указатель на себя
    f_accept( p, currentTime );

    if( texture_ )
    {
        texture_->release();
    }
    vertex_buffer_object_.release();
    vertex_array_object_.release();
    shader_program_.release();
}

bool TFigure::moving() const
{
   if( valid_ )
   {
       return f_moving();
   }
   return false;
}

double TFigure::distance( const CU &point )
{
    return point.distance( offset_.x(), offset_.y(), offset_.z() );
}
