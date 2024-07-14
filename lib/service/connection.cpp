/* 
 * File:   connection.cpp
 * Author: mkh
 * 
 * Created on 2 марта 2023 г., 13:29
 */

#include "connection.h"
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <iostream>

namespace
{
// cетевой адрес в строковый вид
std::string saddr2str( const sockaddr_in & addr )
{
    char buf[64];
    inet_ntop( AF_INET, (char *)&(addr.sin_addr), buf, sizeof(sockaddr_in) );
    return std::string( buf );
}
}  // namespace

TConnection::TConnection( int b_sock )
: fd_( accept( b_sock, (struct sockaddr *)&address_, &socklen_ ) )
{
    // все соединения неблокирующие
    fcntl( fd_, F_SETFD, fcntl( fd_, F_GETFD, 0) | O_NONBLOCK );
    long one;
    if( setsockopt( fd_, SOL_SOCKET, SO_ZEROCOPY, &one, sizeof(one)) )
    {
        send_flags_ = 0;
        std::cerr << "setsockopt zerocopy error: " << strerror( errno ) << std::endl;
    }

    std::cerr << "[+] connected with " << saddr2str(address_) << ":" << ntohs(address_.sin_port) << "\n";
}

TConnection::~TConnection()
{
    // RAII
    close( fd_ );
    std::cerr << "[-] connection " << saddr2str(address_) << ":" << ntohs(address_.sin_port) << " closed\n";
}

// что-то принято из сети
void TConnection::on_data( TBasescreen *screen, const uint8_t * data, int size )
{
    if( !proto_ ) // не определен протокол - начало соединения.
    {
        // может так случиться, что не весь запрос сразу приходит. Поэтому аккумулируем
        request_ += std::string((const char*)data, size);

        // По запросу фабричный метод определит протокол
        TBaseprotocol *p = TBaseprotocol::create( screen, request_, fd_, send_flags_ );
        if( p )
        {
            proto_.reset( p );
        }
    }
    if( proto_ )
    {
        proto_->on_data( data, size );
    }
}

// сеть готова к передаче. Передать, если есть что
void TConnection::on_ready_to_write()
{
    if( proto_ )
    {
        proto_->do_write();
    }
}
