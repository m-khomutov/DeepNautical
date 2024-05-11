/* 
 * File:   qscreen.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 14:43
 */

#include "qscreen.h"
#include "json.h"
#include <QGridLayout>

TQscreen::TQscreen( TBaseframe* frame )
: TGLscreen( frame )
{
    QGridLayout *mainLayout = new QGridLayout;

    // создать сцены (расположение в файле из конфигурации screen_layout), отображаемые экраном
    auto layout = NJson::TParser(NUtils::TConfig()["screen_layout"]).json();
    for( const auto &scene : layout )
    {
        sc_.emplace_back( new TScene( scene.first,
                                      std::string(NUtils::TConfig()["scenes"]) + "/" + scene.first + ".scn",
                                      QSize(frame->width(), frame->height()),
                                      QPoint(scene.second["x"].toInt(), scene.second["y"].toInt())) );
        mainLayout->addWidget( sc_.back(), sc_.back()->position().y(),  sc_.back()->position().x() );
    }

    setLayout(mainLayout);
}

void TQscreen::closeEvent( QCloseEvent *event )
{
    for( auto & scene : sc_ )
    {
        scene->clear();
    }

    QWidget::closeEvent( event );
}

void TQscreen::f_run_scene_display()
{
    // отобразить окно на дисплее
    show();
    // запустить таймер
    timer_id_ = startTimer(0);
}

void TQscreen::f_stop_scene_display()
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
        // выполнить команды, если есть
        f_exec_command();

        for( size_t s(0); s < sc_.size(); ++s )
        {
            sc_[s]->update();
        }

        // сохранить кадр, если время подошло
        if( is_frame_duration_passed( &store_ts_ ) > 0.f )
        {
            TBasescreen::store_scene_frame();
        }
    }
}
