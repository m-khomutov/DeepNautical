/* 
 * File:   water.cpp
 * Author: mkh
 * 
 * Created on 1 февраля 2023 г., 14:18
 */

#include "figures/waves.h"

#include <cmath>
#include <random>

namespace 
{
GLfloat waveGen( GLfloat x, GLfloat z, GLfloat t, Perlin &perlin, const QVector2D &params )
{
    const GLfloat x2 = x - 3;
    const GLfloat z2 = z + 1;
    const GLfloat xx = x2 * x2;
    const GLfloat zz = z2 * z2;
    return (( 2 * (z + 1) * sinf (params[0] * sqrtf (xx + zz) - params[1] * t) + perlin.noise(10 * x, 10 * z, t)) / 250);
}

GLfloat wakeGen( GLfloat x, GLfloat z, GLfloat t, Perlin &perlin, const QVector2D &params )
{
    const GLfloat x2 = x - 3;
    const GLfloat xx = x2 * x2;
    const GLfloat zz = z * z;
    return (sinf ( params[0] * std::sqrt( xx + zz ) - params[1] * t ) + perlin.noise(10 * x, 10 * z, t) ) / 50;
}
}  // namespace


Perlin::Perlin()
{
    std::random_device r;
    std::default_random_engine e1(r());

    std::vector< int > perm( 256 );

    std::iota( perm.begin(), perm.end(), 0 );
    std::shuffle( perm.begin(), perm.end(), e1 );

    for( int i(0); i < 256 ; i++)
    {
        p[256 + i] = p[i] = perm[i];
    }
}

double Perlin::f_fade( double t )
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double Perlin::f_lerp( double t, double a, double b )
{
    return a + t * (b - a);
}

double Perlin::f_grad(int hash, double x, double y, double z)
{
    int h = hash & 15;
    // Convert lower 4 bits of hash into 12 gradient directions
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;

    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

double Perlin::noise(double x, double y, double z)
{
    // Find the unit cube that contains the point
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;
    // Find relative x, y,z of point in cube
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);
    // Compute fade curves for each of x, y, z
    double u = f_fade(x);
    double v = f_fade(y);
    double w = f_fade(z);
    // Hash coordinates of the 8 cube corners
    int A  = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;
    int B  = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;
    // Add blended results from 8 corners of cube
    return f_lerp( w, f_lerp( v, f_lerp( u, f_grad( p[AA], x, y, z ),
                                            f_grad( p[BA], x-1, y, z )),
                                 f_lerp( u, f_grad( p[AB], x, y-1, z ),
                                            f_grad( p[BB], x-1, y-1, z ))),
                      f_lerp( v, f_lerp( u, f_grad( p[AA+1], x, y, z-1 ),
                                      f_grad( p[BA+1], x-1, y, z-1 )),
                                 f_lerp( u, f_grad( p[AB+1], x, y-1, z-1 ),
                                            f_grad( p[BB+1], x-1, y-1, z-1 ))));
}


TWaves::TWaves( const NJson::TObject &environment,const NJson::TObject &settings )
: TFigure( environment, settings )
{
    // проверить настройки
    f_check_environment();
    // сгенерировать геометрическую модель
    model_.setToIdentity();
    model_.translate( QVector3D(0.2f, -0.2f, 0.0f) );
    model_.rotate( 15.f, QVector3D(0.0f, 1.0f, 0.0) );
    model_.rotate( 10.f, QVector3D(1.0f, 0.0f, 0.0) );
    model_.scale( QVector3D(2.7f, 0.9f, 1.4f) );
}

TWaves::~TWaves()
{
    shader_program_.disableAttributeArray("Normals");
    shader_program_.disableAttributeArray("Surface");
}

void TWaves::draw()
{
    shader_program_.setAttributeArray( "Surface", surface_, 4 );
    shader_program_.enableAttributeArray( "Surface" );
    shader_program_.setAttributeArray( "Normals", normals_, 3 );
    shader_program_.enableAttributeArray( "Normals" );

    // настроить униформные переменные шейдера
    shader_program_.setUniformValue( "Offset", offset_ );
    shader_program_.setUniformValue( "LightPosition", QVector4D(spec_.light_position, 1.0) );
    shader_program_.setUniformValue( "LightColor", QVector4D(spec_.light_color, 1.0) );
    shader_program_.setUniformValue( "CameraPosition", spec_.camera_position );
    shader_program_.setUniformValue( "FogParams.color", spec_.fog_color );
    shader_program_.setUniformValue( "FogParams.density", spec_.fog_density );

    // активизировать текстуры
    if( air_texture_ )
    {
        shader_program_.setUniformValue( "AirTexture", GLuint(1) );
        air_texture_->bind(1);
    }
    if( foam_texture_ )
    {
        shader_program_.setUniformValue( "FoamTexture", GLuint(2) );
        foam_texture_->bind(2);
    }

    // отрисовать поверхность
    uint32_t len = 2 * (resolution + 1);
    for( uint32_t i(0); i < resolution; i++ )
    {
        glDrawArrays( GL_TRIANGLE_STRIP, i * len, len );
    }

    if( air_texture_ )
    {
        air_texture_->release();
    }
    if( foam_texture_ )
    {
        foam_texture_->release();
    }
    shader_program_.disableAttributeArray("Normals");
    shader_program_.disableAttributeArray("Surface");
}

void TWaves::set_wake_position( const std::vector< TFigure::TPosition > &pos )
{
    wake_position_ = pos;   
}

void TWaves::f_check_environment() const
{
    if( ! (NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/vert_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["shaders"]) + "/frag_" + spec_.shader_name).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["textures"]) + "/"     + spec_.texture_name).c_str()) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["textures"]) + "/"     + spec_.texture_air).c_str() ) &&
           NUtils::file_exists( (std::string(NUtils::TConfig()["textures"]) + "/"     + spec_.texture_foam).c_str() )) )
    {
        throw  std::runtime_error( std::string("invalid environment in {") + spec_.shader_name + " " + spec_.texture_name + "}"  );
    }
}

char const *TWaves::f_shader_name() const
{
    return spec_.shader_name.c_str(); 
}

void TWaves::f_initialize()
{
    // создать текстуры
    //std::string alpha = spec_.alpha.empty() ? "" : std::string(NUtils::TConfig()["textures"]) + "/" + spec_.alpha;
    texture_.reset( new QOpenGLTexture( QImage(std::string(std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_name).c_str() ) ) );
    air_texture_.reset( new QOpenGLTexture( QImage(std::string(std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_air).c_str() ) ) );
    foam_texture_.reset( new QOpenGLTexture( QImage(std::string(std::string(NUtils::TConfig()["textures"]) + "/" + spec_.texture_foam).c_str() ) ) );
}

void TWaves::f_accept( IVisitor &p, double currentTime )
{
    f_load_surface( currentTime );
    p.visit( this );
}

void TWaves::f_load_surface( double )
{
    phase_ += 0.01f;
    if( phase_ >= 360.0f )
    {
        phase_ = 0.0f;
    }

    // сгенерить координаты точек поверхности
    const float delta = 2.7f / resolution;
    for( uint32_t z(0); z < resolution; ++z )
    {
        GLfloat z_coord = z * delta - 1.0f; 
        for( uint32_t x(0); x <= resolution; ++x )
        {
            GLfloat x_coord = x * delta - 1.0f;
            int idx = 8 * (x + z * (resolution + 1));
            surface_[idx + 4] = ( x_coord );
            surface_[idx + 5] = f_generate_surface( x_coord, z_coord, &surface_[idx + 7] );
            surface_[idx + 6] = z_coord;
            if( z )
            {
                int pidx = 8 * (x + (z - 1) * (resolution + 1));
                surface_[idx + 0] = surface_[pidx + 4];
                surface_[idx + 1] = surface_[pidx + 5];
                surface_[idx + 2] = surface_[pidx + 6];
                surface_[idx + 3] = surface_[pidx + 7];
            }
            else
            {
                surface_[idx + 0] = ( x_coord );
                surface_[idx + 1] = f_generate_surface( x_coord, z_coord, &surface_[idx + 3] );
                surface_[idx + 2] = z_coord;
            }

            // и их нормали
            int n_idx = 6 * (x + z * (resolution + 1));
            QVector3D a( surface_[idx + 4], surface_[idx + 5],  surface_[idx + 6] );
            QVector3D b( surface_[idx + 4], surface_[idx + 1],  surface_[idx + 2] );
            QVector3D c( surface_[idx + 12], surface_[idx + 13], surface_[idx + 6] );
            if( x >= resolution )
            {
                const float xn = (resolution + 1) * delta + 1;
                c = QVector3D( xn, f_generate_surface( xn, a.z() ), a.z() );
            }
            QVector3D n = QVector3D::crossProduct(c - a, b - a);
            n.normalize();
            normals_[n_idx + 3] = n.x();
            normals_[n_idx + 4] = n.y();
            normals_[n_idx + 5] = n.z();

            if( z )
            {
                int pidx = 6 * (x + (z - 1) * (resolution + 1));
                normals_[n_idx + 0] = normals_[pidx + 3];
                normals_[n_idx + 1] = normals_[pidx + 4];
                normals_[n_idx + 2] = normals_[pidx + 5];
            }
            else
            {
                a[1] = f_generate_surface( a[0], (z - 1) * delta - 1 );
                a[2] = (z - 1) * delta - 1;

                c[1] = f_generate_surface( c[0], b[2] );
                c[2]= b[2];

                n = QVector3D::crossProduct(c - a, b - a);
                n.normalize();
                normals_[n_idx + 0] = n.x();
                normals_[n_idx + 1] = n.y();
                normals_[n_idx + 2] = n.z();
            }
        }
    }
}

GLfloat TWaves::f_generate_surface(GLfloat x, GLfloat z, GLfloat *in_wake)
{
    if( in_wake )
    {
        *in_wake = 0.f;
    }
    GLfloat rc = waveGen( x, z, phase_, perlin_, spec_.wave );
    for( auto pos : wake_position_ )
    {
        if( pos.course.x() ) {
            bool coords_inverted = pos.course.x() < 0; // движения слева направо по экрану
            bool in_x_trace = coords_inverted ? ( x < pos.current.x() ) : x > -pos.current.x();
            GLfloat wake_width = coords_inverted ? pos.wake[0] - x * pos.wake[1]
                                                 : pos.wake[0] + x * pos.wake[1];
            bool in_z_trace = ( z > pos.current.z() - wake_width && z < pos.current.z() );
            if( in_x_trace && in_z_trace )
            {
                rc += wakeGen( x, z, coords_inverted ? phase_ : -phase_, perlin_, spec_.wake );
                if( in_wake )
                {
                    *in_wake = 1.0;
                }
            }
        }
    }
    return rc;
}
