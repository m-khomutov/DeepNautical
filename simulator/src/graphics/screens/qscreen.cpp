/* 
 * File:   qscreen.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 14:43
 */

#include "qscreen.h"
#include <QDateTime>
#include <QApplication>

qscreen::qscreen( baseframe* frame )
: QOpenGLWidget(nullptr)
, frame_( frame )
{
}

qscreen::~qscreen()
{
}

void qscreen::run()
{
    show();
    update_id_ = startTimer(0);
}

void qscreen::stop()
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
    sc_.reset(new scene);
    glViewport( 0, 0, width(), height() );
}

void qscreen::paintGL()
{
    sc_->display( width(), height(), QDateTime::currentMSecsSinceEpoch());
    frame_->store( width(), height());
}

void qscreen::resizeGL(int w, int h)
{
    glViewport( 0, 0, w, h);
}

void qscreen::error_cb( int error, const GLchar * description )
{
    throw screen_error( std::string(description) );
}
