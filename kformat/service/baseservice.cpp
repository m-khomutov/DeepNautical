/* 
 * File:   base.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 16:14
 */

#include "baseservice.h"
#include "screen/basescreen.h"

TBaseservice::TBaseservice( TBasescreen *screen, uint16_t port )
: screen_( screen )
, poll_( screen, port )
, poll_thread_( &poll_ ) // поток работы с сетью стартует автоматом
{}

// поток работы с сетью автоматом не стартует. Надо запускать функцией start_vdev_capture
TBaseservice::TBaseservice( char const *videodevname, uint16_t port )
: poll_( videodevname, port )
{}

TBaseservice::~TBaseservice()
{}

void TBaseservice::start_screen()
{
    // проверить, может запуск потока с прослушкой сети не сработал
    std::string what = poll_thread_.exception();
    if( !what.empty() )
    {
        throw std::runtime_error( what );
    }

    // вроде норм. Стартуем экран отображения
    if( screen_ )
    {
        screen_->run_scene_display();

        f_start_screen();
    }
}

int TBaseservice::stop_screen()
{
    if( screen_ )
    {
        screen_->stop_scene_display();
    }

    return f_stop_screen();
}

void TBaseservice::start_vdev_capture()
{
    // poll владеет объектом работы с файлом видеозахвата.
    poll_.start_listening_network();
}

void TBaseservice::stop_vdev_capture()
{
    poll_.stop_listening_network();
}
