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
        throw screen_error("initialize GL init error");
    }
    for( int i(0); i < utils::config()["scene_count"]; ++i )
    {
        if( scene_iter_ == scenes_.end() )
        {
            throw screen_error("not enough scenes to start");
        }
        sc_.emplace_back( new scene( std::string(utils::config()["scenes"]) + "/" + *scene_iter_ + ".scn" ) );
        ++scene_iter_;
    }

    viewes_ = 0;
    for( auto &s : sc_ )
    {
        viewes_ += s->cameras();
    }

    resize( viewes_ * frame_->width(), frame_->height() );
}

void qscreen::paintGL()
{
    f_exec_command();

    glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    for( size_t s(0), view(0); s < sc_.size(); ++s )
    {
        for( size_t v( 0 ); v < sc_[s]->cameras(); ++v )
        {
            glViewport( view * frame_->width(), 0, frame_->width(), height() );
            sc_[s]->display( v, frame_->width(), height(), QDateTime::currentMSecsSinceEpoch());
            ++view;
        }
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
