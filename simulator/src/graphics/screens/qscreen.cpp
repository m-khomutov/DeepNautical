/* 
 * File:   qscreen.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 14:43
 */

#include "qscreen.h"
#include <QDateTime>
#include <GL/glew.h>

qscreen::qscreen( TBaseframe* frame )
: TGLscreen( frame )
, QOpenGLWidget(nullptr)
{
    resize( frame_->width(), frame_->height() );
}

qscreen::~qscreen()
{}

void qscreen::f_run_scene_display()
{
    show();
    update_id_ = startTimer(0);
}

void qscreen::f_stop_scene_display()
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
    {
        update();
    }
}

void qscreen::initializeGL()
{
    glewExperimental = GL_TRUE;
    if( glewInit() != GLEW_OK )
    {
        throw TGLscreenError("initialize GL init error");
    }
    for( int i(0); i < NUtils::TConfig()["scene_count"]; ++i )
    {
        if( scene_iter_ == scenes_.end() )
        {
            throw TGLscreenError("not enough scenes to start");
        }
        sc_.emplace_back( new scene( *scene_iter_, std::string(NUtils::TConfig()["scenes"]) + "/" + *scene_iter_ + ".scn" ) );
        ++scene_iter_;
    }

    view_count_ = 0;
    for( auto &s : sc_ )
    {
        view_count_ += s->cameras();
    }

    resize( view_count_ * frame_->width(), frame_->height() );
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

    if( is_frame_duration_passed( &store_ts_ ) > 0.f )
    {
        TBasescreen::store_scene_frame();
    }
}

void qscreen::resizeGL(int w, int h)
{
    glViewport( 0, 0, w, h);
}

void qscreen::error_cb( int error, const GLchar * description )
{
    Q_UNUSED( error );
    throw TGLscreenError( std::string(description) );
}
