/* 
 * File:   s_poll.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 10:06
 */

#include "loop.h"
#include "connection.h"
#include "screen.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <iostream>


looperror::looperror( const std::string &what )
    : std::runtime_error( what + std::string(" failed: ") + std::string(strerror( errno )) )
{}


loop::loop( char const *videodevname, uint16_t port )
    : socket_( port )
    , fd_( epoll_create( 1 ) )
    , vdev_( new vdev( videodevname ) )
{
    try
    {
        f_add_socket( socket_, EPOLLIN | EPOLLOUT | EPOLLET );
    }
    catch( const std::runtime_error & err )
    {
        close( fd_ );
        throw;
    }
}

loop::loop( base::screen *screen, uint16_t port )
: socket_( port )
, fd_( epoll_create( 1 ) )
, screen_( screen )
{
    try
    {
        f_add_socket( socket_, EPOLLIN | EPOLLOUT | EPOLLET );
    }
    catch( const std::runtime_error & err )
    {
        close( fd_ );
        throw;
    }
}

loop::~loop()
{
    close( fd_ );
}

void loop::start()
{
    epoll_event events[maxevents];
    base::frame::time_point_t last_ts = std::chrono::high_resolution_clock::now();
    
    uint8_t buffer[0xffff];
    while( running_.load() )
    {
        int fd_count = epoll_wait( fd_, events, maxevents, 10 );
        for( int i(0); i < fd_count; ++i )
        {
            if( events[i].data.fd == socket_ )
            {
                std::shared_ptr< connection > conn( new connection( events[i].data.fd ) );
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
                if( vdev_ )
                {
                    vdev_->start();
                }
            }
            else if( events[i].events & EPOLLIN )
            {
                int rc = ::read( events[i].data.fd, buffer, sizeof(buffer) );
                if( rc > 0 )
                {
                    auto p = connections_.find( events[i].data.fd );
                    if( p != connections_.end() )
                    {
                        p->second->on_data( screen_, buffer, rc );
                    }
                }
            }
            else if( events[i].events & EPOLLOUT )
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
            if( events[i].events & (EPOLLRDHUP | EPOLLHUP) )
            {
                epoll_ctl( fd_, EPOLL_CTL_DEL, events[i].data.fd, nullptr );
                connections_.erase( events[i].data.fd );
                if( vdev_ && connections_.empty() )
                {
                    vdev_->stop();
                }
            }
        }
        f_send_frame( &last_ts );
    }
}

void loop::stop()
{
    running_.store( false );
}

void loop::f_add_socket( int sock, uint32_t events )
{
    epoll_event ev;
    ev.events = events;
    ev.data.fd = sock;
    if( epoll_ctl( fd_, EPOLL_CTL_ADD, sock, &ev ) == -1 )
    {
        throw looperror( "epoll_ctl" );
    }
}

void loop::f_send_frame( base::frame::time_point_t * last_ts )
{
    float duration = screen_ ? screen_->frame_expired( last_ts ) : (vdev_ ? vdev_->frame_expired( last_ts ) : -1.f);

    if( duration > 0.f )
    {
        for( auto p : connections_ )
        {
            if( p.second->protocol() && p.second->protocol()->can_send_frame() ) // кадр отправляется
            {
                if( screen_ )
                {
                    if( !screen_->send_frame( p.second->protocol() ) && p.second->protocol() )
                    {
                        p.second->protocol()->write_error();
                    }
                }
                if( vdev_ )
                {
                    vdev_->send_frame( p.second->protocol() );
                }
            }
        }
    }
}
