/* 
 * File:   s_poll.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 10:06
 */

#include "s_poll.h"
#include "connection.h"
#include "screen/basescreen.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <iostream>


TSpollError::TSpollError( const std::string &what )
: std::runtime_error( what + std::string(" failed: ") + std::string(strerror( errno )) )
{}


// принимает кадры с платы видеозахвата
TSpoll::TSpoll( char const *videodevname, uint16_t port )
: p_socket_( port )
, fd_( epoll_create( 1 ) )
, videodev_( new TVideodevice( videodevname ) )
{
    try
    {
        // слушающий входные соединения tcp-сокет
        f_add_socket( p_socket_, EPOLLIN | EPOLLOUT | EPOLLET );
    }
    catch( const std::runtime_error & err )
    {
        close( fd_ );
        throw;
    }
}

// принимает кадры с экрана сцен
TSpoll::TSpoll( TBasescreen *screen, uint16_t port )
: p_socket_( port )
, fd_( epoll_create( 1 ) )
, screen_( screen )
{
    try
    {
        // слушающий входные соединения tcp-сокет
        f_add_socket( p_socket_, EPOLLIN | EPOLLOUT | EPOLLET );
    }
    catch( const std::runtime_error & err )
    {
        close( fd_ );
        throw;
    }
}

TSpoll::~TSpoll()
{
    // RAII
    close( fd_ );
}

void TSpoll::start_listening_network()
{
    epoll_event events[maxevents];
    // сюда копирует данные из сетевого буфера
    uint8_t buffer[0xffff];
    // метка времени, следящая за превышением длительности видеокадра
    TBaseframe::time_point_t last_ts = std::chrono::high_resolution_clock::now();
    
    while( running_.load() )
    {
        int fd_count = epoll_wait( fd_, events, maxevents, 10 ); // ждем запросов
        for( int i(0); i < fd_count; ++i )
        {
            if( events[i].data.fd == p_socket_ )  // запрос на создание соединения
            {
                std::shared_ptr< TConnection > conn( new TConnection( events[i].data.fd ) );
                try
                {
                    f_add_socket( *conn, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLHUP );
                }
                catch( const std::runtime_error &err )
                {
                    std::cerr << "error: " <<err.what() << std::endl;
                    continue;
                }
                connections_[*conn] = conn;
                if( videodev_ ) // если кадры с платы захвата, можно начинать их читать - есть абонент получения
                {
                    videodev_->start_capture();
                }
            }
            else if( events[i].events & EPOLLIN ) // запрос на прием созданном соединении
            {
                int rc = ::read( events[i].data.fd, buffer, sizeof(buffer) ); // читаем
                if( rc > 0 )
                {
                    auto p = connections_.find( events[i].data.fd );
                    if( p != connections_.end() )
                    {
                        p->second->on_data( screen_, buffer, rc );
                    }
                }
            }
            else if( events[i].events & EPOLLOUT ) // разрешение на передачу на созданных соединениях
            {
                auto p = connections_.find( events[i].data.fd );
                if( p != connections_.end() )
                {
                    p->second->on_ready_to_write();
                }
            }
            else
            {
                std::cerr << "[?] unexpected event\n";
            }
            // соединение закрывается абонентом
            if( events[i].events & (EPOLLRDHUP | EPOLLHUP) )
            {
                epoll_ctl( fd_, EPOLL_CTL_DEL, events[i].data.fd, nullptr );
                connections_.erase( events[i].data.fd );
                if( videodev_ && connections_.empty() ) // если нет абонентов получать кадры с платы не для кого
                {
                    videodev_->stop_capture();
                }
            }
        }
        // отправить кадр желающим
        f_send_frame( &last_ts );
    }
}

void TSpoll::stop_listening_network()
{
    running_.store( false );
}

void TSpoll::f_add_socket( int sock, uint32_t events )
{
    epoll_event ev;
    ev.events = events;
    ev.data.fd = sock;
    if( epoll_ctl( fd_, EPOLL_CTL_ADD, sock, &ev ) == -1 )
    {
        throw TSpollError( "epoll_ctl" );
    }
}

// отправить кадр абонентам, в случае превышения заданной длительности кадра
void TSpoll::f_send_frame( TBaseframe::time_point_t * last_ts )
{
    // длтельность кадра хранится в объекте экрана сцен или в объекте работы с платой видеозахвата
    float duration = screen_ ? screen_->is_frame_duration_passed( last_ts ) : (videodev_ ? videodev_->is_frame_duration_passed( last_ts ) : -1.f);

    if( duration > 0.f ) // длительность предыдущего кадра превышена
    {
        for( auto p : connections_ )
        {
            if( p.second->protocol() && p.second->protocol()->can_send_frame() ) // кадр отправляется
            {
                if( screen_ )
                {
                    if( !screen_->send_stored_scene_frame( p.second->protocol() ) && p.second->protocol() )
                    {
                        // не отправился. Возвращаем ошибку
                        p.second->protocol()->write_error();
                    }
                }
                if( videodev_ )
                {
                    videodev_->send_frame( p.second->protocol() );
                }
            }
        }
    }
}
