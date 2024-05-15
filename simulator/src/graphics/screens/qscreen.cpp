/* 
 * File:   qscreen.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 14:43
 */

#include "qscreen.h"
#include "json.h"
#include <QGridLayout>

TQscreen::TQscreen()
{
    QGridLayout *mainLayout = new QGridLayout;

    // создать сцены (расположение в файле из конфигурации screen_layout), отображаемые экраном
    const auto &layout = NUtils::scene_config_t(NUtils::TConfig()["screen_layout"]);
    for( const auto &scene : layout )
    {
        sc_.emplace_back( new TScene( scene.name,
                                      std::string(NUtils::TConfig()["scenes"]) + "/" + scene.name + ".scn",
                                      QSize(scene.size.width, scene.size.height),
                                      QPoint(scene.x, scene.y) ) );
        mainLayout->addWidget( sc_.back(), scene.y,  scene.x );
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
void TQscreen::timerEvent( QTimerEvent * )
{
    if( timer_id_ != -1 )
    {
        // выполнить команды, если есть
        f_exec_command();

        for( size_t s(0); s < sc_.size(); ++s )
        {
            sc_[s]->update();
        }
    }
}
