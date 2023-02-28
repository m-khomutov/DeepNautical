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
#include <cstring>
#include <fstream>
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

scene::scene( const std::string &specification )
{
    glEnable( GL_DEBUG_OUTPUT );
    glEnable( GL_DEPTH_TEST );
    glDepthMask( GL_TRUE );
    glDepthFunc( GL_LEQUAL );
    glDepthRange( 0.0f, 1.0f );
    glDebugMessageCallback( scene::debugCb, this );
    glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    f_debug_info();
    f_initialize( specification );
}

scene::~scene()
{
}

void scene::display( GLuint width, GLuint height, double currentTime )
{
    glClearColor( 0.392f, 0.706f, 0.983f, 1.0f );
    glClearDepth(1.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    figureset_.draw( currentTime );
}

void scene::f_initialize( const std::string &specification )
{
    std::vector< std::string > settings;
    std::string header;
    utils::read_config( specification.c_str(), [&]( const std::string &line ) {
        if( line[ 0 ] == '[' )
        {
            if( !header.empty() )
            {
                f_add_figure( header, settings );
                settings.clear();
            }
            header = line;
        }
        else
        {
            settings.push_back( line );
        }
    });
    if( !header.empty() )
    {
        f_add_figure( header, settings );
    }
    figureset_.initialize();
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

void scene::f_add_figure( const std::string &header, const std::vector< std::string > &settings)
{
    if( header == "[Horizon]" )
    {
        f_add_figure< horizon >( settings );
    }
    else if( header == "[Vessel]" )
    {
        f_add_figure< vessel >( settings );
    }
}

template< typename Figure >
void scene::f_add_figure( const std::vector< std::string > &settings )
{
    try
    {
        figureset_.emplace( new Figure( settings ) );
    }
    catch( const std::runtime_error &err )
    {
        std::cerr << __PRETTY_FUNCTION__ << " error: " <<err.what() <<std::endl;
    }
}
