/* 
 * File:   baseprotocol.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 14:59
 */

#include "baseprotocol.h"
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <iostream>

protocol_error::protocol_error( const std::string &what )
: std::runtime_error( what + std::string(" failed: ") + std::string(strerror( errno )) )
{
}

namespace
{
std::string saddr2str( const sockaddr_in & addr )
{
    char buf[64];
    inet_ntop( AF_INET, (char *)&(addr.sin_addr), buf, sizeof(sockaddr_in) );
    return std::string( buf );
}
}

baseprotocol::baseprotocol( int b_sock )
: fd_( accept( b_sock, (struct sockaddr *)&address_, &socklen_ ) )
{
    fcntl( fd_, F_SETFD, fcntl( fd_, F_GETFD, 0) | O_NONBLOCK );
    std::cerr << "[+] connected with " << saddr2str(address_) << ":" << ntohs(address_.sin_port) << "\n";
}

baseprotocol::~baseprotocol()
{
    close( fd_ );
    std::cerr << "[-] connection " << saddr2str(address_) << ":" << ntohs(address_.sin_port) << " closed\n";
}
