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


s_poll_error::s_poll_error( const std::string &what )
: std::runtime_error( what + std::string(" failed: ") + std::string(strerror( errno )) )
{
}


s_poll::s_poll( char const *videodevname, uint16_t port )
: p_socket_( port )
, fd_( epoll_create( 1 ) )
, videodev_( new videodevice( videodevname ) )
{
    try
    {
        f_add( p_socket_, EPOLLIN | EPOLLOUT | EPOLLET );
    }
    catch( const std::runtime_error & err )
    {
        close( fd_ );
        throw;
    }
}

s_poll::s_poll( basescreen *screen, uint16_t port )
: p_socket_( port )
, fd_( epoll_create( 1 ) )
, screen_( screen )
{
    try
    {
        f_add( p_socket_, EPOLLIN | EPOLLOUT | EPOLLET );
    }
    catch( const std::runtime_error & err )
    {
        close( fd_ );
        throw;
    }
}

s_poll::~s_poll()
{
    close( fd_ );
}

void s_poll::run()
{
    epoll_event events[maxevents];
    uint8_t buffer[0xffff];
    baseframe::time_point_t last_ts = std::chrono::high_resolution_clock::now();
    
    while( running_.load() )
    {
        int fd_count = epoll_wait( fd_, events, maxevents, 10 );
        for( int i(0); i < fd_count; ++i )
        {
            if( events[i].data.fd == p_socket_ )
            {
                std::shared_ptr< connection > conn( new connection( events[i].data.fd ) );
                try
                {
                    f_add( *conn, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLHUP );
                }
                catch( const std::runtime_error &err )
                {
                    std::cerr << "error: " <<err.what() << std::endl;
                    continue;
                }
                connections_[*conn] = conn;
                videodev_->start();
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
            /* check if the connection is closing */
            if( events[i].events & (EPOLLRDHUP | EPOLLHUP) )
            {
                epoll_ctl( fd_, EPOLL_CTL_DEL, events[i].data.fd, nullptr );
                connections_.erase( events[i].data.fd );
                if( connections_.empty() )
                {
                    videodev_->stop();
                }
            }
        }
        f_send_frame( &last_ts );
    }
}

void s_poll::stop()
{
    running_.store( false );
}

void s_poll::f_add( int sock, uint32_t events )
{
    epoll_event ev;
    ev.events = events;
    ev.data.fd = sock;
    if( epoll_ctl( fd_, EPOLL_CTL_ADD, sock, &ev ) == -1 )
    {
        throw s_poll_error( "epoll_ctl" );
    }
}

void s_poll::f_send_frame( baseframe::time_point_t * last_ts )
{
    float duration = screen_ ? screen_->frame_duration_passed( last_ts ) : videodev_->frame_duration_passed( last_ts );
    if( duration > 0.f )
    {
        for( auto p : connections_ )
        {
            if( p.second->protocol() && p.second->protocol()->can_send_frame() )
            {
                if( screen_ )
                {
                    if( !screen_->load( p.second->protocol(), duration ) && p.second->protocol() )
                    {
                        p.second->protocol()->write_error();
                    }
                }
                if( videodev_ )
                {
                    videodev_->load( p.second->protocol(), duration );
                }
            }
        }
    }
}
