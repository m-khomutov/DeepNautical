/* 
 * File:   scene.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 12:07
 */

#include "scene.h"
#include "figures/sol.h"
#include "figures/water.h"
#include "figures/horizon.h"
#include "figures/antisubmarinefrigate.h"
#include "figures/vessel.h"
#include <iostream>

namespace {
    std::string severity2str( GLuint severity ) {
        switch( severity ) {
            case GL_DEBUG_SEVERITY_HIGH:
                return "Ошибка или опасная ситуация";
            case GL_DEBUG_SEVERITY_MEDIUM:
                return "Вожное предупреждение";
            case GL_DEBUG_SEVERITY_LOW:
                return "Предупреждение";
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                return "Извещение";
        }
        return std::to_string( severity );
    }
}


scene_error::scene_error( const std::string &what )
: std::runtime_error( what )
{
}

void GLAPIENTRY
scene::debugCb( GLenum src,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei sz,
                const GLchar * msg,
                const void * p )
{
    reinterpret_cast< const scene * >(p)->f_debug_error( src, type, id, severity, std::string(msg, sz) );
}

scene::scene()
{
    glEnable( GL_DEBUG_OUTPUT );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_ALWAYS );
    glDebugMessageCallback( scene::debugCb, this );
    glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    f_debug_info();
    
    if( utils::config()["graphicsdim"] == utils::graphicsdim::dim2D )
    {
        f_add_sol();
        f_add_water();
        f_add_antisubmarinefrigate();
    }
    else
    {
        f_add_horizon();
        f_add_vessel();
    }
    figureset_.initialize();
}

scene::~scene()
{
}

void scene::display( GLuint width, GLuint height, double currentTime )
{
    glClearColor( 0.392f, 0.706f, 0.983f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    figureset_.draw( currentTime );
}

void scene::f_debug_info()
{
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cerr << "OpenGL: {\n\tvendor: " << glGetString( GL_VENDOR )
              << "\n\trenderer: " << glGetString( GL_RENDERER )
              << "\n\tversion: " << glGetString( GL_VERSION ) << " (" << major << "." << minor << ")"
              << "\n\tGLSL version: " << glGetString( GL_SHADING_LANGUAGE_VERSION )
              << "\n\textensions: [ ";
    GLint nExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);
    for( int i = 0; i < nExtensions; i++ )
        std::cerr<< glGetStringi( GL_EXTENSIONS, i ) << " ";
    std::cerr << "]\n}" << std::endl;   
}

void scene::f_debug_error( GLenum src,
                           GLenum type,
                           GLuint id,
                           GLenum severity,
                           std::string msg ) const
{
    std::cerr << src << ":" << type <<"[" << severity2str(severity) <<"](" <<id <<") :" << msg << std::endl;
}

void scene::f_add_sol()
{
    if( ! sol::environment_valid() )
    {
        std::cerr << "sol environment is invalid\n";
        return;
    }
    try
    {
        figureset_.emplace( new sol );
    }
    catch( const std::runtime_error &err )
    {
        std::cerr << "sol error: " <<err.what() <<std::endl;
    }
}

void scene::f_add_water()
{
    if( ! water::environment_valid() )
    {
        std::cerr << "water environment is invalid\n";
        return;
    }
    try
    {
        figureset_.emplace( new water );
    }
    catch( const std::runtime_error &err )
    {
        std::cerr << "water error: " <<err.what() <<std::endl;
    }
}

void scene::f_add_horizon()
{
    if( ! horizon::environment_valid() )
    {
        std::cerr << "horizon environment is invalid\n";
        return;
    }
    try
    {
        figureset_.emplace( new horizon );
    }
    catch( const std::runtime_error &err )
    {
        std::cerr << "horizon error: " <<err.what() <<std::endl;
    }
}

void scene::f_add_antisubmarinefrigate()
{
    if( ! antisubmarinefrigate::environment_valid() )
    {
        std::cerr << "antisubmarinefrigate environment is invalid\n";
        return;
    }
    try
    {
        figureset_.emplace( new antisubmarinefrigate );
    }
    catch( const std::runtime_error &err )
    {
        std::cerr << " antisubmarinefrigate error: " <<err.what() <<std::endl;
    }
}

void scene::f_add_vessel()
{
    if( ! vessel::environment_valid() )
    {
        std::cerr << "vessel environment is invalid\n";
        return;
    }
    try
    {
        figureset_.emplace( new vessel );
    }
    catch( const std::runtime_error &err )
    {
        std::cerr << "vessel error: " <<err.what() <<std::endl;
    }
}
