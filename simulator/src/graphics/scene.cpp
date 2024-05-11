/* 
 * File:   scene.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 12:07
 */

#include "scene.h"
#include "utils.h"

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include "figures/sol.h"
#include "figures/water.h"
//#include "figures/horizon.h"
//#include "figures/vessel.h"
//#include "figures/sparklets.h"

#include <QDateTime>
#include <QOpenGLDebugLogger>

#include <cstring>
#include <fstream>
#include <iostream>
#include <list>

TSceneError::TSceneError( const std::string &what )
: std::runtime_error( what )
{}

TScene::TScene( const std::string &name, const std::string &specification, QSize size, QPoint pos, QWidget *parent )
: QGLWidget( QGLFormat(), parent )
, name_( name )
, size_( size )
, position_( pos )
{
    f_initialize( specification );
}

TScene::~TScene()
{
    clear();
}

QSize TScene::sizeHint() const
{
    return size_;
}

const QPoint& TScene::position() const
{
    return position_;
}

void TScene::initializeGL()
{
    glEnable( GL_DEBUG_OUTPUT );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_FRONT );
    glFrontFace( GL_CW );
    glDepthMask( GL_TRUE );
    glDepthFunc( GL_LEQUAL );
    glDepthRange( 0.0f, 1.0f );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glClearColor( 0.392f, 0.706f, 0.983f, 1.0f );
    glClearDepth(1.0f);

    f_debug_info();
    // инициализируем фигуры в контейнере
    figureset_.initialize();
}

void TScene::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    try
    {
        figureset_.draw( QDateTime::currentMSecsSinceEpoch() );
    }
    catch( const std::runtime_error &err )
    {
        std::cerr << __PRETTY_FUNCTION__ << " error: " << err.what() <<std::endl;
    }
}

void TScene::resizeGL(int w, int h)
{
    if (h == 0)
    {
        h = 1;
    }
    glViewport(0, 0, w, h);
}

void TScene::clear()
{
    /* Qt doesn't guarantee that the context is current at clean up time
       so we must explicitly make that so by calling the makeCurrent() function before doing any clean up.
     */
    makeCurrent();

    figureset_.clear();

    doneCurrent();
}

void TScene::onMessageLogged( QOpenGLDebugMessage message )
{
    if( message.severity() < QOpenGLDebugMessage::NotificationSeverity )
    {
        qDebug() << message;
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

    // создаем сами фигуры
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
            f_add_figure( figure.first, param );
        }
    }
}

void TScene::f_debug_info()
{
    // информация по системе GL
    glGetIntegerv(GL_MAJOR_VERSION, &GL_major_);
    glGetIntegerv(GL_MINOR_VERSION, &GL_minor_);
    if( position_.x() == 0 && position_.y() == 0 )
    {
        qDebug() << "OpenGL: {\n\tvendor: " << (char*)glGetString( GL_VENDOR )
                 << "\n\trenderer: " << (char*)glGetString( GL_RENDERER )
                 << "\n\tversion: " << (char*)glGetString( GL_VERSION ) << " (" << GL_major_ << "." << GL_minor_ << ")"
                 << "\n\tGLSL version: " << (char*)glGetString( GL_SHADING_LANGUAGE_VERSION )
                 << "\n\textensions: [ ";
        GLint nExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);
        qDebug() << (char*)glGetString( GL_EXTENSIONS );
        qDebug() << "]\n}";
    }
    f_initialize_debugging();
}

void TScene::f_initialize_debugging()
{
    QSurfaceFormat format;
    format.setMajorVersion( GL_major_ );
    format.setMinorVersion( GL_minor_ );
    format.setProfile( QSurfaceFormat::CoreProfile );
    format.setOption( QSurfaceFormat::DebugContext );

    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    ctx->setFormat(format);

    QOpenGLDebugLogger *logger = new QOpenGLDebugLogger( this );
    logger->initialize();
    connect( logger, &QOpenGLDebugLogger::messageLogged, this, &TScene::onMessageLogged );
    logger->startLogging();
}

void TScene::f_add_figure( const std::string &header, const std::vector< std::string > &settings )
{
    // строка хедера определяет тип фигуры
/*    if( header == "[Horizon]" )
    {
        f_add_figure< THorizon >( settings, camera_pos );
    }
    else if( header == "[Vessel]" )
    {
        f_add_figure< TVessel >( settings, camera_pos );
    }
    else*/ if( header == "[Water]" )
    {
        f_add_figure< TWater >( settings );
    }
    else if( header == "[Sol]" )
    {
        f_add_figure< TSol >( settings );
    }
    /*else if( header == "[Sparklets]" )
    {
        f_add_figure< TSparklets >( settings, camera_pos );
    }*/
}

template< typename Figure >
void TScene::f_add_figure( const std::vector< std::string > &settings )
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
