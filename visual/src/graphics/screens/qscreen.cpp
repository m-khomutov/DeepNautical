/* 
 * File:   qscreen.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 14:43
 */

#include "qscreen.h"
#include <QDateTime>
#include <GL/glew.h>

TQscreen::TQscreen( base::frame* frame )
: TGLscreen( frame )
, QOpenGLWidget(nullptr)
{
    // выставить размеры окна по размерам кадра
    resize( frame_->width(), frame_->height() );
}

void TQscreen::f_run()
{
    // отобразить окно на дисплее
    show();
    // запустить таймер
    timer_id_ = startTimer(0);
}

void TQscreen::f_stop()
{
    // остановить таймер
    if( timer_id_ != -1 )
    {
        killTimer( timer_id_ );
    }
    timer_id_ = -1;
}

// Обработка события таймера
void TQscreen::timerEvent(QTimerEvent *)
{
    if( timer_id_ != -1 )
    {
        update(); // запустить отрисовку окна
    }
}

void TQscreen::initializeGL()
{
    // проинициализировать контекст GL
    glewExperimental = GL_TRUE;
    if( glewInit() != GLEW_OK )
    {
        throw TGLscreenError("initialize GL init error");
    }
    // создать сцены (количество = scene_count из конфигурации), отображаемые экраном
    for( int i(0); i < utils::settings()["scene_count"]; ++i )
    {
        // контролировать наличие запрошенного количества
        if( scene_iter_ == scenes_.end() )
        {
            throw TGLscreenError("not enough scenes to start");
        }
        // добавить сцену
        sc_.emplace_back( new TScene( *scene_iter_, std::string(utils::settings()["scenes"]) + "/" + *scene_iter_ + ".scn" ) );
        ++scene_iter_;
    }

    // посчитать количество точек обзора (сумма камер на всем сценах)
    view_count_ = 0;
    for( auto &s : sc_ )
    {
        view_count_ += s->cameras();
    }
    // изменить размер окна с учетом количества точек обзора
    resize( view_count_ * frame_->width(), frame_->height() );
}

void TQscreen::paintGL()
{
    // выполнить команды, если есть
    f_exec_command();
    // очистить GL  контекст
    glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    // отрисовать все сцены
    for( size_t s(0), view(0); s < sc_.size(); ++s )
    {
        // со всех точек обзора
        for( size_t v( 0 ); v < sc_[s]->cameras(); ++v )
        {
            glViewport( view * frame_->width(), 0, frame_->width(), height() );
            sc_[s]->display( v, frame_->width(), height(), QDateTime::currentMSecsSinceEpoch());
            ++view;
        }
    }
    // сохранить кадр, если время подошло
    if( frame_expired( &store_ts_ ) > 0.f )
    {
        base::screen::update_frame();
    }
}

void TQscreen::resizeGL(int w, int h)
{
    // размеры окна изменились. Надо изменить GL вьюпорт
    glViewport( 0, 0, w, h);
}
