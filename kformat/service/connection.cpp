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
std::string saddr2str( const sockaddr_in & addr )
{
    char buf[64];
    inet_ntop( AF_INET, (char *)&(addr.sin_addr), buf, sizeof(sockaddr_in) );
    return std::string( buf );
}
}  // namespace

connection::connection( int b_sock )
: fd_( accept( b_sock, (struct sockaddr *)&address_, &socklen_ ) )
{
    fcntl( fd_, F_SETFD, fcntl( fd_, F_GETFD, 0) | O_NONBLOCK );
    std::cerr << "[+] connected with " << saddr2str(address_) << ":" << ntohs(address_.sin_port) << "\n";
}

connection::~connection()
{
    close( fd_ );
    std::cerr << "[-] connection " << saddr2str(address_) << ":" << ntohs(address_.sin_port) << " closed\n";
}

void connection::on_data( const uint8_t * data, int size )
{
    if( !proto_ )
    {
        request_ += std::string((const char*)data, size);
        baseprotocol *p = baseprotocol::create( request_, fd_ );
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

void connection::on_ready_to_write()
{
    if( proto_ )
    {
        proto_->do_write();
    }
}
