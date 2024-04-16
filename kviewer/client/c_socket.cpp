/* 
 * File:   c_socket.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 18:04
 */

#include "c_socket.h"

#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>


TCsocketError::TCsocketError( const std::string &what )
: std::runtime_error( what + std::string(" failed: ") + std::string(strerror( errno )) )
{}

TCsocket::TCsocket( std::string const &addr, uint16_t port )
: fd_( ::socket( AF_INET, SOCK_STREAM, 0) )
{
    // разрешить переиспользование
    long yes { 0 };
    setsockopt( fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) );

    // сделать сокет неблокирующим
    fcntl( fd_, F_SETFL, fcntl(fd_, F_GETFL, 0) | O_NONBLOCK );

    // соединиться с удаленным сервером
    sockaddr_in ca = { AF_INET, htons(port), { f_resolve_host( addr ) }, { 0 } };
    int rc = connect( fd_, (sockaddr*)&ca, sizeof(sockaddr_in) );
    if( rc == -1 )
    {
        if( errno != EINPROGRESS )  // неудача
        {
            throw TCsocketError( "connect" );
        }
        fd_set wfds;
        timeval tv = { 5, 0 };
        FD_ZERO( &wfds );
        FD_SET( fd_, &wfds );
        if( (rc = select( fd_ + 1, nullptr, &wfds, nullptr, &tv )) < 0 && errno != EINTR )
        {
            throw TCsocketError( "connect" );
        }
        else if( (rc > 0 ) )
        {
            socklen_t slen = sizeof( int );
            int nok;
            if( getsockopt( fd_, SOL_SOCKET, SO_ERROR, (void*)(&nok), &slen ) < 0 || nok )
            {
                throw TCsocketError( "check connection" );
            }
        }
    }
}

TCsocket::~TCsocket()
{
    close( fd_ );
}

ssize_t TCsocket::send( uint8_t const * buf, size_t size )
{
    size_t rc { 0 };
    while( rc < size )
    {
        int bs = ::send( fd_, &buf[rc], size - rc, 0 );
        if( bs > 0 )
        {
            rc += bs;
        }
        else if( bs < 0 )
        {
            if( !(errno == EAGAIN || errno == EINPROGRESS) )
            {
                throw TCsocketError( "send" );
            }
        }
    }
    return rc;
}

ssize_t TCsocket::receive( uint8_t *buffer, size_t size )
{
    timeval tv = { 1, 500 };
    fd_set rfds;

    FD_ZERO( &rfds );
    FD_SET ( fd_, &rfds );
    if( select( fd_ + 1, &rfds, nullptr, nullptr, &tv ) <= 0 || !FD_ISSET( fd_, &rfds ) )
        return 0;
    
    return ::recv( fd_, buffer, size, 0 );
}

in_addr_t TCsocket::f_resolve_host( std::string const &host )
{
    hostent *hp = gethostbyname( host.c_str() );
    if (hp) {
        int i = 0;
        while( hp -> h_addr_list[i] )
        {
            return (*(in_addr*)( hp->h_addr_list[ i ] ) ).s_addr;
            ++ i;
        }
    }
    throw TCsocketError( std::string("invalid host: ") + host );
}
