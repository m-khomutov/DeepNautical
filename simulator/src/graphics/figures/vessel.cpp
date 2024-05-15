/* 
 * File:   vessel.cpp
 * Author: mkh
 * 
 * Created on 16 февраля 2023 г., 13:11
 */

#include "vessel.h"

TVessel::TVessel( const std::vector< std::string > &settings )
: TFigure( settings )
{
    // проверить настройки
    f_check_environment();

    // создать объект геометрической фигуры (координаты сетки фигуры)
    object_.reset( new NBlender::TObject( (std::string(NUtils::TConfig()["objs"]) + "/" + spec_.obj_name).c_str() ) );
    object_->load_position( &vertices_ );

    f_reset();
}

void TVessel::draw()
{
    try
    {
        shader_program_.setAttributeArray( "Position", vertices_.data(),                       3, 8 * sizeof(GLfloat) );
        shader_program_.enableAttributeArray( "Position" );
        shader_program_.setAttributeArray( "Texcoord", vertices_.data() + 3 * sizeof(GLfloat), 2, 8 * sizeof(GLfloat) );
        shader_program_.enableAttributeArray( "Texcoord" );
        shader_program_.setAttributeArray( "Normal",   vertices_.data() + 5 * sizeof(GLfloat), 3, 8 * sizeof(GLfloat) );
        shader_program_.enableAttributeArray( "Normal" );
    }
    catch( const std::runtime_error &e )
    {
        qDebug() << "vessel error: " << e.what();
        return;
    }

    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    // настроить общие униформные переменные в шейдере
    shader_program_.setUniformValue( "NormalMatrix", (view_ * model_).normalMatrix() );
    shader_program_.setUniformValue( "LightPosition", spec_.light_position );
    shader_program_.setUniformValue( "LightColor", spec_.light_color );
    shader_program_.setUniformValue( "CameraPosition", spec_.camera_position );
    shader_program_.setUniformValue( "fog.color", spec_.fog_color );
    shader_program_.setUniformValue( "fog.density", spec_.fog_density );

    GLuint first = 0; // счетчик поверхностей фигуры. За один проход рисуется одна поверхность
    for( auto mtl : object_->materials() )
    {
        /*try
        {
            // униформные переменные мателиала фигуры
            program_->uniform_block("Material" )["Ka"].set( mtl.Ka );
            program_->uniform_block("Material" )["Kd"].set( mtl.Kd );
            program_->uniform_block("Material" )["Ks"].set( mtl.Ks );
            program_->uniform_block("Material" )["Ns"].set( mtl.Ns );
            program_->uniform_block("Material" )["Ni"].set( mtl.Ni );
            program_->uniform_block("Material" )["d"].set( mtl.d );
            program_->uniform_block("Material" )["illum"].set( mtl.illum );
            program_->uniform_block("Material").copy();
        }
        catch( const std::runtime_error &e )
        {
            std::cerr << "Material error: " << e.what() << std::endl;
        }*/
        try
        {
            // текстура фигуры
            if( mtl.map_Kd )
            {
                mtl.map_Kd->bind();
            }
            else
            {
                empty_texture_->bind();
            }
            shader_program_.setUniformValue( "Texture", GLuint(0) );
        }
        catch( const std::runtime_error &e )
        {
            qDebug() << "Texture error: " << e.what();
        }
        // нарисовать фигуру на сцене
        glDrawArrays( GL_TRIANGLES, first, mtl.faces.size() * 3 );
        first += mtl.faces.size() * 3;
    }

    shader_program_.disableAttributeArray("Normal");
    shader_program_.disableAttributeArray("Texcoord");
    shader_program_.disableAttributeArray("Position");
}

const TFigure::TPosition &TVessel::position()
{
    return position_;
}

void TVessel::f_check_environment() const
{
    if( ! (NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["objs"]) + "/" + spec_.obj_name).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + " " + spec_.obj_name + "}"  );
    }
}

char const *TVessel::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void TVessel::f_initialize()
{
    // пустая текстура. Если объект не будет иметь своей, будем пользовать эту
    empty_texture_.reset( new QOpenGLTexture( QImage(std::string(std::string(NUtils::TConfig()["textures"]) + "/" + spec_.empty_texture).c_str() ) ) );
}

void TVessel::f_accept( IVisitor &p, double )
{
    f_set_model();
    p.visit( this );
}

void TVessel::f_set_model()
{
    // Качнуть фигуру
    pitching_angle_ += spec_.pitching;
    if( pitching_angle_ < spec_.pitching_range[0] || pitching_angle_ > spec_.pitching_range[1] )
    {
        spec_.pitching *= -1;
    }
    // установить фигуру на сцене
    model_.setToIdentity();
    model_.rotate( spec_.course.z() + angle_.z(), QVector3D(0.0f, 0.0f, 1.0) );
    model_.rotate( spec_.course.y() + angle_.y(), QVector3D(0.0f, 1.0f, 1.0) );
    model_.rotate( spec_.course.x() + angle_.x(), QVector3D(1.0f, 0.0f, 1.0) );
    model_.translate( QVector3D(offset_.x(), offset_.y(), offset_.z()) );
    model_.scale( factor_ );
    if( ! f_full_trajectory() )
    {
        // изменить следующее положение и скорость
        offset_ += spec_.speed;
        factor_ += spec_.factor_gain;
        position_.current.setX( position_.current.x() + spec_.speed.x() );
        position_.current.setY( position_.current.y() + spec_.speed.y() );
        position_.current.setZ( spec_.speed.z() * (spec_.course.x() < 0 ? -1 : 1) );
    }
    else
    {
        f_reset();
    }
    // изменить следующий угол крена
    angle_ += spec_.angle_gain;
    angle_.setX( angle_.x() + spec_.pitching );
}

bool TVessel::f_full_trajectory()
{
    if( spec_.speed.x() < 0.0f )
    {
        return spec_.start_position.x() - offset_.x() >= spec_.trajectory;
    }
    return offset_.x() - spec_.start_position.x() >= spec_.trajectory;
}

void TVessel::f_reset()
{
    offset_ = spec_.start_position; 
    factor_ = spec_.start_factor;
    angle_ = QVector3D( 0.0f, 0.0f, 0.0f );
    pitching_angle_ = spec_.pitching_range[0];
    angle_.setX( pitching_angle_ );
    
    f_set_model();

    position_.current = QVector3D(offset_.x(), offset_.y(), offset_.z());
    position_.course = spec_.course;
    position_.wake = spec_.wake_width;
}
