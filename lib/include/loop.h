/*!
     \file s_poll.h
     \author mkh
     \date 25 января 2023 г.
     \brief Заголовочный файл класса создания сетевых соединений с абонентом.

     Данный файл содержит в себе состояние класса создания сетевых соединений с абонентом, объявление его интерфейсов.
 */

#ifndef S_POLL_H
#define S_POLL_H

#include "tcpsock.h"
#include "vdev.h"
#include "frame.h"

#include <atomic>
#include <chrono>
#include <memory>
#include <map>

namespace base
{

class screen;

}  // namespace base

class connection;

class looperror: public std::runtime_error
{
public:
    looperror( const std::string & what );
};

class loop {
public:
    loop( base::screen *screen, uint16_t port );
    loop( char const *videodevname, uint16_t port );
    loop( const loop& other ) = delete;
    ~loop();

    loop &operator =( const loop& other ) = delete;

    void start();
    void stop();

private:
    enum { maxevents = 32 };
    
    std::atomic< bool > running_ { true };
    tcpsock socket_;
    int fd_;

    base::screen *screen_ { nullptr };
    std::unique_ptr< vdev > vdev_;

    std::map< int, std::shared_ptr< connection > > connections_;

private:
    void f_add_socket( int sock, uint32_t events );
    void f_send_frame( base::frame::time_point_t *last_ts );
};

#endif /* S_POLL_H */
