/* 
 * File:   qscreen.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 14:43
 */

#include "qscreen.h"
#include <QDateTime>
#include <GL/glew.h>

qscreen::qscreen( baseframe* frame )
: openglscreen( frame )
, QOpenGLWidget(nullptr)
{
    resize( frame_->width(), frame_->height() );
}

qscreen::~qscreen()
{}

void qscreen::f_run()
{
    show();
    update_id_ = startTimer(0);
}

void qscreen::f_stop()
{
    if(update_id_ != -1)
    {
        killTimer(update_id_);
    }
    update_id_ = -1;
}

void qscreen::timerEvent(QTimerEvent *)
{
    if( update_id_ != -1 )
    update();
}

void qscreen::initializeGL()
{
    glewExperimental = GL_TRUE;
    if( glewInit() != GLEW_OK )
    {
        throw screen_error("GLEW init error");
    }
    sc_.reset(new scene( std::string(utils::config()["scenes"]) + "/" + *scene_iter_ + ".scn" ) );

    viewes_ = sc_->cameras();
    resize( viewes_ * frame_->width(), frame_->height() );
}

void qscreen::paintGL()
{
    f_exec_command();

    for( size_t v(0); v < viewes_; ++v )
    {
        glViewport( v * frame_->width(), 0, frame_->width(), height() );
        sc_->display( v, frame_->width(), height(), QDateTime::currentMSecsSinceEpoch());
    }

    if( frame_duration_passed( &store_ts_ ) > 0.f )
    {
        basescreen::store();
    }
}

void qscreen::resizeGL(int w, int h)
{
    glViewport( 0, 0, w, h);
}

void qscreen::error_cb( int error, const GLchar * description )
{
    Q_UNUSED( error );
    throw screen_error( std::string(description) );
}
