/* 
 * File:   connection.cpp
 * Author: mkh
 * 
 * Created on 2 марта 2023 г., 13:29
 */

#include "connection.h"
#include "httpprotocol.h"
#include "flvprotocol.h"
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
        f_determine_protocol( data, size );
    }
    else
    {
        proto_->on_data( data, size );
    }
}

void connection::send_frame( const uint8_t * data, int size, float duration )
{
    if( proto_ )
    {
        proto_->send_frame( data, size, duration );
    }
}

void connection::f_determine_protocol( const uint8_t * data, int size )
{
    request_ += std::string( (const char*)data, size );
    if( request_.find( "\r\n\r\n" ) != std::string::npos )
    {
        if( request_.find( "GET /stream?proto=flv HTTP/1.1\r\n" ) != std::string::npos )
        {
            proto_.reset( new flvprotocol( fd_ ) );
        }
	else
        {
            proto_.reset( new httpprotocol( fd_ ) );
        }
        proto_->on_data( (const uint8_t*)request_.data(), request_.size() );
    }
}
