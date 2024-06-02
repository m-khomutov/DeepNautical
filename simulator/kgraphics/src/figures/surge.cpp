/* 
 * File:   sparklets.cpp
 * Author: mkh
 * 
 * Created on 21 августа 2023 г., 14:49
 */

#include "figures/surge.h"

TSurge::TSurge( const NJson::TObject &environment,const NJson::TObject &settings )
: TFigure( environment, settings )
, sparkles_vertex_buffer_object_( QOpenGLBuffer(QOpenGLBuffer::VertexBuffer) )
{
    // проверить настройки
    f_check_environment();
    // настроить геометрию точек случайными координатами
    f_set_points();
    // настроить модель
    model_.setToIdentity();
    model_.translate( QVector3D(0.f, 0.f, -1.0f) );
    model_.rotate( -60.f, QVector3D(1.0f, 0.0f, 0.0) );
}

TSurge::~TSurge()
{
    sparkles_vertex_buffer_object_.destroy();
}

void TSurge::draw()
{
    shader_program_.setUniformValue( "Time", last_frame_time_ );

    f_draw_layout();    // подложка
    f_draw_sparklets(); // точки - блики

    if( air_texture_ )
    {
        air_texture_->release();
    }
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
    f_initialize_sparklets();
}

void TSurge::f_accept( IVisitor &p, double )
{
    last_frame_time_ += spec_.speed.x();
    p.visit( this );
}

void TSurge::f_initialize_layout()
{
    vertex_buffer_object_.allocate( spec_.viewport.data(), spec_.viewport.size() * sizeof(GLfloat) );

    shader_program_.enableAttributeArray( "WavePosition" );
    shader_program_.setAttributeBuffer( "WavePosition", GL_FLOAT, 0, 3, 5 * sizeof(GLfloat) );

    shader_program_.enableAttributeArray( "Texcoord" );
    shader_program_.setAttributeBuffer( "Texcoord", GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    // создать текстуру
    //std::string alpha = std::string(NUtils::TConfig()["textures"]) + "/" + spec_.alpha;
    texture_.reset( new QOpenGLTexture( QImage(std::string(std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_name).c_str() ) ) );
    air_texture_.reset( new QOpenGLTexture( QImage(std::string(std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_air).c_str() ) ) );
}

void TSurge::f_initialize_sparklets()
{

    sparkles_vertex_buffer_object_.setUsagePattern( QOpenGLBuffer::DynamicDraw );
    sparkles_vertex_buffer_object_.create();
    sparkles_vertex_buffer_object_.bind();

    sparkles_vertex_buffer_object_.allocate( points_.data(), points_.size() * sizeof(GLfloat) );

    shader_program_.enableAttributeArray( "SparklePosition" );
    shader_program_.setAttributeBuffer( "SparklePosition", GL_FLOAT, 0, 3, 6 * sizeof(GLfloat) );

    shader_program_.enableAttributeArray( "SparkleColor" );
    shader_program_.setAttributeBuffer( "SparkleColor", GL_FLOAT, 3 * sizeof(GLfloat), 3, 6 * sizeof(GLfloat));

    sparkles_vertex_buffer_object_.release();
}

void TSurge::f_draw_layout()
{
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
    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
}

void TSurge::f_draw_sparklets()
{
    sparkles_vertex_buffer_object_.bind();

    shader_program_.setUniformValue( "DrawSparkles", 1.f );

    glDrawArrays( GL_POINTS, 0, points_.size() / 6 );

    f_reset_points();
}

void TSurge::f_set_points()
{
    // случайные геометрические положения точек-бликов и их цветов
    for ( float x = spec_.viewport[0]; x < spec_.viewport[5]; x += spec_.step ) {
        //for ( float y = spec_.viewport[11]; y < spec_.viewport[1]; y += spec_.step, z += spec_.step ) {
        for ( float y = -1.5f; y < -0.1f; y += 0.2f ) {
            points_.push_back(((float)rand() / RAND_MAX) * 0.1 + x);
            points_.push_back(((float)rand() / RAND_MAX) * 0.1 + y);
            points_.push_back(((float)rand() / RAND_MAX) * 0.1);

            points_.push_back((float(rand()) / RAND_MAX) + 0.5);
            points_.push_back((float(rand()) / RAND_MAX) + 0.5);
            points_.push_back((float(rand()) / RAND_MAX) + 0.5);
        }
    }

    sparkles_vertex_buffer_object_.write( 0, points_.data(), points_.size() * sizeof(GLfloat) );
}

void TSurge::f_reset_points()
{
    float *ptr = points_.data();
    // случайные геометрические положения точек-бликов и их цветов
    for ( float x = spec_.viewport[0]; x < spec_.viewport[5]; x += spec_.step ) {
        //for ( float y = spec_.viewport[11]; y < spec_.viewport[1]; y += spec_.step, z += spec_.step ) {
        for ( float y = -1.5f; y < -0.1f; y += 0.2f ) {
            *ptr++ = ((float)rand() / RAND_MAX) * 0.1 + x;
            *ptr++ = ((float)rand() / RAND_MAX) * 0.1 + y;
            *ptr++ = ((float)rand() / RAND_MAX) * 0.1;
            
            *ptr++ = float(rand()) / RAND_MAX + 0.5;
            *ptr++ = float(rand()) / RAND_MAX + 0.5;
            *ptr++ = float(rand()) / RAND_MAX + 0.5;
        }
    }

    sparkles_vertex_buffer_object_.write( 0, points_.data(), points_.size() * sizeof(GLfloat) );
}
