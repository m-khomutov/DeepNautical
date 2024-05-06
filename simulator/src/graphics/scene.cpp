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
#include "figures/vessel.h"
#include "figures/sparklets.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>

namespace {
    // текстовое представление серьезности ошибки в системе GL
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


TSceneError::TSceneError( const std::string &what )
: std::runtime_error( what )
{}

// регистрируется в GL для вывода отладки. Переадресует информацию в объект сцены
void GLAPIENTRY
TScene::debugCb( GLenum src,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei sz,
                 const GLchar * msg,
                 const void * p )
{
    reinterpret_cast< const TScene * >(p)->f_debug_error( TDebugMessage().set_source( src )
                                                                         .set_type( type )
                                                                         .set_id( id )
                                                                         .set_severity( severity )
                                                                         .set_body( msg, sz  ) );
}

TScene::TScene( const std::string &name, const std::string &specification )
: name_( name )
{
    glEnable( GL_DEBUG_OUTPUT );
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_CULL_FACE);
    glDepthMask( GL_TRUE );
    glDepthFunc( GL_LEQUAL );
    glDepthRange( 0.0f, 1.0f );
    glDebugMessageCallback( TScene::debugCb, this );
    glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glClearColor( 0.392f, 0.706f, 0.983f, 1.0f );
    glClearDepth(1.0f);

    f_debug_info();
    f_initialize( specification );
}

void TScene::display( size_t view, GLuint/* width*/, GLuint/* height*/, double currentTime )
{
    if( view < figureset_.size() )
    {
        figureset_[view]->draw( currentTime );
    }
}

void TScene::f_initialize( const std::string &specification )
{
    using param_t = std::vector< std::string >;

    // фигура - тип (хедер) + набор параметров
    std::map< std::string/*header*/, std::list< param_t > > figures;
    param_t settings;
    std::string header;
    // читаем спецификацию
    NUtils::read_config( specification.c_str(), [&]( const std::string &line ) {
        if( line[ 0 ] == '[' ) // новый блок
        {
            if( !header.empty() ) // уже не первый - есть прочитанная фигура
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

    // данные из блока Environment расшариваются по всем фигурам
    const std::list< param_t > &environment_params = figures["[Environment]"];
    // получаем координаты камер на сцене
    std::vector< glm::vec3 > camera_positions = f_get_camera_positions( environment_params.back() );

    // создаем сами фигуры
    for( auto camera_pos : camera_positions )
    {
        figureset_.emplace_back( new TFigureset() ); // каждой точке обзора свой контейнер объектов

        for( auto figure : figures )
        {
            for( param_t &param : figure.second )
            {
                if( !(environment_params.empty() || figure.first == "[Environment]") )
                {
                    // добавить параметры Environment, которые для всех фигур
                    param.insert( param.end(), environment_params.back().begin(), environment_params.back().end() );
                }
                // следующая фигура в контейнер
                f_add_figure( figure.first, param, camera_pos );
            }
        }
        // инициализируем фигуры в контейнере
        figureset_.back()->initialize();
    }
}

void TScene::f_debug_info()
{
    // информация по системе GL
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

void TScene::f_debug_error( const TDebugMessage &msg ) const
{
    // информация из сообщения системы GL
    std::cerr << msg.source << ":"
              << msg.type << "["
              << severity2str( msg.severity ) << "]("
              << msg.id << ") : "
              << msg.body << std::endl;
}

void TScene::f_add_figure( const std::string &header, const std::vector< std::string > &settings, const glm::vec3 &camera_pos )
{
    // строка хедера определяет тип фигуры
    if( header == "[Horizon]" )
    {
        f_add_figure< THorizon >( settings, camera_pos );
    }
    else if( header == "[Vessel]" )
    {
        f_add_figure< TVessel >( settings, camera_pos );
    }
    else if( header == "[Water]" )
    {
        f_add_figure< TWater >( settings, camera_pos );
    }
    else if( header == "[Sol]" )
    {
        f_add_figure< TSol >( settings, camera_pos );
    }
    else if( header == "[Sparklets]" )
    {
        f_add_figure< TSparklets >( settings, camera_pos );
    }
}

template< typename Figure >
void TScene::f_add_figure( const std::vector< std::string > &settings, const glm::vec3 &camera_pos )
{
    try
    {
        figureset_.back()->emplace( new Figure( settings, camera_pos ) );
    }
    catch( const std::runtime_error &err )
    {
        std::cerr << __PRETTY_FUNCTION__ << " error: " << err.what() <<std::endl;
    }
}

std::vector< glm::vec3 > TScene::f_get_camera_positions( const std::vector< std::string > &settings )
{
    std::vector< glm::vec3 > positions;

    // найти поле camera_position и распарсить содержимое
    for( auto s : settings )
    {
        std::pair< std::string, std::string > p;
        if( NUtils::str2key( s, &p ) )
        {
            if( p.first.find( "camera_position" ) != std::string::npos )
            {
                glm::vec3 next_position;
                size_t p1 = 2, p2 = p.second.find("} {");
                while( p2 != std::string::npos )
                {
                    if( !NUtils::str2vec( p.second.substr( p1, p2 - p1 ), &next_position ) )
                    {
                        std::cerr << "invalid camera position: " << p.second << "\n";
                        break;
                    }
                    positions.push_back( next_position );
                    p1 = p2 + 3;
                    p2 = p.second.find("} {", p1);
                }
                p2 = p.second.find("}}", p1 );
                if( p2 != std::string::npos )
                {
                    if( NUtils::str2vec( p.second.substr( p1, p2 - p1 ), &next_position ) )
                    {
                        positions.push_back( next_position );
                    }
                }
            }
        }
    }
    // нету камер с спеке. Ну пусть одна такая будет. Совсем без камер нельзя
    if( positions.empty() )
    {
        positions.push_back( glm::vec3( 0.0f, 0.0f, 5.0f ) );
    }

    return positions;
}
