/* 
 * File:   scene.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 12:07
 */

#include "scene.h"
#include "utils.h"

#include <QMouseEvent>

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include "figures/air.h"
#include "figures/waves.h"
#include "figures/surge.h"
#include "figures/vessel.h"
#include "figures/skybox.h"

#include <QDateTime>
#include <QOpenGLDebugLogger>

#include <cstring>
#include <fstream>
#include <iostream>
#include <list>

TSceneError::TSceneError( const std::string &what )
: std::runtime_error( what )
{}

TScene::TScene( const NUtils::TSceneConfig &config, QWidget *parent )
: QOpenGLWidget( parent )
, name_( config.name )
, size_( config.size.width, config.size.height )
, position_( config.x, config.y )
, store_ts_( std::chrono::high_resolution_clock::now() )
, camera_( QVector3D(config.specification["environment"]["camera-position"]["x"].toFloat(),
                     config.specification["environment"]["camera-position"]["y"].toFloat(),
                     config.specification["environment"]["camera-position"]["z"].toFloat()),
           width(),
           height() )
{
    designationCoords_[0] = { (350. - 180.), .0, 4000. };
    designationCoords_[0].normalize();

    f_initialize( config.specification );
}

TScene::~TScene()
{
    clear();
}

QSize TScene::minimumSizeHint() const
{
    return size_;
}

const QPoint& TScene::position() const
{
    return position_;
}

void TScene::initializeGL()
{
    initializeOpenGLFunctions();

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

    frame_.reset( new TJpegframe( NUtils::TGeometry(width(), height()),
                                  NUtils::TConfig()["compress_quality"],
                                  NUtils::TConfig()["frame_duration"] ) );

    f_debug_info();
    // инициализируем фигуры в контейнере
    makeCurrent();

    figureset_.initialize();

    doneCurrent();
}

void TScene::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera_.move( width(), height() );

    try
    {
        figureset_.draw( QDateTime::currentMSecsSinceEpoch(), camera_ );
    }
    catch( const std::runtime_error &err )
    {
        qDebug() << __PRETTY_FUNCTION__ << " error: " << err.what();
    }

    f_capture_target();

    // сохранить кадр, если время подошло
    if( frame_->is_duration_passed() > 0.f )
    {
        f_store_frame();
    }
}

void TScene::resizeGL(int w, int h)
{
    if( h == 0 )
    {
        h = 1;
    }
    glViewport(0, 0, w, h);
}

void TScene::f_capture_target()
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    if( panorama_ == 1 )
    {
        camera_.rotate( 0.f, -0.09 * (currentTime - capture_time_) );
    }
    else if( panorama_ == 2 )
    {
        camera_.rotate( 0.f, -0.045 * (currentTime - capture_time_) );
    }

    if( capture_mode_ == 1 )
    {
        QVector3D rotation( designationCoords_[0].elevation_deg, designationCoords_[0].bearing_deg, 0.f );

        captured_target_ = figureset_.closest_on_distance( designationCoords_[0], 10. );
        CU target_coords = captured_target_ ? CU(captured_target_->coordinates()) : designationCoords_[0];

        camera_.setRotation( rotation.x() - target_coords.elevation_deg,
                             rotation.y() - target_coords.bearing_deg,
                             0.f );
    }
    else if( capture_mode_ == 2 )
    {
        CU operateCoords { camera_.rotation().y() - 180., camera_.rotation().x() + 180., 0. };

        operateCoords.normalize();

        if( !captured_target_ )
        {
            captured_target_ = figureset_.closest_on_distance( operateCoords, 10. );
        }

        if( captured_target_ )
        {
            CU target_coords = captured_target_->coordinates();

            camera_.setRotation( camera_.rotation().x() - target_coords.elevation_deg,
                                 camera_.rotation().y() - target_coords.bearing_deg,
                                 0.f );

            qDebug() << "Print  1 " << target_coords.elevation_deg;
        }
    }

    camera_.normalize();

    QString mess = "FPS: " + QString::number(1000.0/(currentTime - capture_time_)) +
            "\nOne frame:" + QString::number(currentTime - capture_time_) + "msec";

    // Render text
    QPainter painter(this);
    painter.beginNativePainting();
    painter.drawText( QRect(10,10,100,100), mess );
    painter.endNativePainting();
    painter.end();

    capture_time_ = currentTime;
}

void TScene::mousePressEvent( QMouseEvent *event )
{
    if( event->buttons() == Qt::LeftButton )
    {
        mouse_pos_ = event->pos();
    }
}

void TScene::mouseMoveEvent( QMouseEvent *event )
{
    camera_.rotate( TCamera::DEG * (event->pos().y() - mouse_pos_.y()) / height(),
                    TCamera::DEG * (event->pos().x() - mouse_pos_.x()) / width() );
    mouse_pos_ = event->pos();
}

void TScene::mouseReleaseEvent( QMouseEvent *event )
{
    if( event->buttons() == Qt::RightButton )
    {
        emit clicked();
    }
}

void TScene::wheelEvent( QWheelEvent *e )
{
    camera_.zoom( camera_.fieldOfView() - e->delta() / 60 );
}

void TScene::keyPressEvent( QKeyEvent *e )
{
    if( e->key() == Qt::Key_M )
    {
        //figureset_.setColorMode( cl_mode_ );
        cl_mode_ = (cl_mode_ + 1) % 3;
    }
    else if( e->key() == Qt::Key_N )
    {
        panorama_ = (panorama_ + 1) % 3;
    }
    else if( e->key() == Qt::Key_X )
    {
        capture_mode_ = 0;
        captured_target_ = nullptr;
    }
    else if( e->key() == Qt::Key_C )
    {
        capture_mode_ = 1;
    }
    else if( e->key() == Qt::Key_V )
    {
        capture_mode_ = 2;
    }
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

void TScene::zoomCamera( float factor )
{
    camera_.zoom( factor );
}

void TScene::rotateCamera( float x, float y )
{
    camera_.rotate( x, y );
}

void TScene::setPanorama( int factor )
{
    panorama_ = factor;
}

void TScene::f_store_frame()
{
    std::lock_guard< std::mutex > lk(frame_mutex_);

    // получить указатель на буфер кадра. Явное нарушение инкапсуляции. На что не пойдешь ради оптимизации
    uint8_t *buffer = frame_->buffer( frame_->width(), frame_->height() );

    // прочесть в буфер текущий кадр
    int q;
    glGetIntegerv( GL_READ_BUFFER, &q );
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    glReadPixels( 0, 0, frame_->width(), frame_->height(), GL_RGB, GL_UNSIGNED_BYTE, buffer );

    // подготовить (сжать) буфер для последующей передачи
    frame_->prepare_buffer();
}

bool TScene::send_frame( TBaseprotocol *proto )
{
    std::lock_guard< std::mutex > lk(frame_mutex_);

    if( frame_->is_updated() )
    {
        return frame_->send_buffer( proto );
    }
    return false;
}

void TScene::onMessageLogged( QOpenGLDebugMessage message )
{
    if( message.severity() < QOpenGLDebugMessage::NotificationSeverity )
    {
        qDebug() << message;
    }
}

void TScene::f_initialize( const NJson::TObject &specification )
{
    // данные из блока environment расшариваются по всем фигурам
    NJson::TObject environment;
    try {
        environment = specification["environment"];

        camera_.setPosition( environment["camera-position"]["x"].toInt(),
                             environment["camera-position"]["y"].toInt(),
                             environment["camera-position"]["z"].toInt() );
    }
    catch( std::exception &e )
    {
        qDebug() << e.what();
    }
    // читаем спецификацию
    const NJson::TObject &figures = specification["figures"];
    for( const auto &fig : figures )
    {
        f_add_figure( environment, fig.second );
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

void TScene::f_add_figure( const NJson::TObject &environment, const NJson::TObject &settings )
{
    // строка хедера определяет тип фигуры
    if( std::string(settings["type"]) == "vessel" )
    {
        f_add_figure< TVessel >( environment, settings );
    }
    else if( std::string(settings["type"]) == "waves" )
    {
        f_add_figure< TWaves >( environment, settings );
    }
    else if( std::string(settings["type"]) == "air" )
    {
        f_add_figure< TAir >( environment, settings );
    }
    else if( std::string(settings["type"]) == "surge" )
    {
        f_add_figure< TSurge >( environment, settings );
    }
    else if( std::string(settings["type"]) == "skybox" )
    {
        f_add_figure< TSkybox >( environment, settings );
    }
}

template< typename Figure >
void TScene::f_add_figure( const NJson::TObject &environment,const NJson::TObject &settings )
{
    try
    {
        figureset_.emplace( new Figure( environment, settings ) );
    }
    catch( const std::runtime_error &err )
    {
        qDebug() << __PRETTY_FUNCTION__ << " error: " << err.what();
    }
}
