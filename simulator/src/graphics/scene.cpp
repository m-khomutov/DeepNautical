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
#include "figures/sol.h"
#include "figures/vessel.h"
#include "figures/sparklets.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>

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
    reinterpret_cast< const scene * >(p)->f_debug_error( debug_message().set_source( src )
                                                                        .set_type( type )
                                                                        .set_id( id )
                                                                        .set_severity( severity )
                                                                        .set_body( msg, sz  ) );
}

scene::scene( const std::string &specification )
{
    glEnable( GL_DEBUG_OUTPUT );
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_CULL_FACE);
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

void scene::display( GLuint/* width*/, GLuint/* height*/, double currentTime )
{
    glClearColor( 0.392f, 0.706f, 0.983f, 1.0f );
    glClearDepth(1.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    figureset_.draw( currentTime );
}

void scene::f_initialize( const std::string &specification )
{
    using param_t = std::vector< std::string >;

    std::map< std::string/*header*/, std::list< param_t > > figures;
    param_t settings;
    std::string header;
    utils::read_config( specification.c_str(), [&]( const std::string &line ) {
        if( line[ 0 ] == '[' )
        {
            if( !header.empty() )
            {
                figures[header].push_back( settings );
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
        figures[header].push_back( settings );
    }

    const std::list< param_t > &environment_params = figures["[Environment]"];
    for( auto figure : figures )
    {
        for( param_t &param : figure.second )
        {
            if( !(environment_params.empty() || figure.first == "[Environment]") )
            {
                param.insert( param.end(), environment_params.back().begin(), environment_params.back().end() );
            }
            f_add_figure( figure.first, param );
        }
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

void scene::f_debug_error( const debug_message &msg ) const
{
    std::cerr << msg.source << ":"
              << msg.type << "["
              << severity2str( msg.severity ) << "]("
              << msg.id << ") : "
              << msg.body << std::endl;
}

void scene::f_add_figure( const std::string &header, const std::vector< std::string > &settings )
{
    if( header == "[Horizon]" )
    {
        f_add_figure< horizon >( settings );
    }
    else if( header == "[Vessel]" )
    {
        f_add_figure< vessel >( settings );
    }
    else if( header == "[Water]" )
    {
        f_add_figure< water >( settings );
    }
    else if( header == "[Sol]" )
    {
        f_add_figure< sol >( settings );
    }
    else if( header == "[Sparklets]" )
    {
        f_add_figure< sparklets >( settings );
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
        std::cerr << __PRETTY_FUNCTION__ << " error: " << err.what() <<std::endl;
    }
}
