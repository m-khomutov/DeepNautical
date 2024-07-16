/* 
 * File:   socket.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 9:24
 */

#include "tcpsock.h"

#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>

tcpsockerror::tcpsockerror( const std::string &what )
    : std::runtime_error( what + std::string(" failed: ") + std::string(strerror( errno )) )
{}


tcpsock::tcpsock( uint16_t b_port )
{
    sockaddr_in b_addr;
    memset( &b_addr, 0, sizeof(struct sockaddr_in) );
    b_addr.sin_family = AF_INET;
    b_addr.sin_addr.s_addr = INADDR_ANY;
    b_addr.sin_port = htons(b_port);

    if( (fd_ = socket( b_addr.sin_family, SOCK_STREAM, 0 )) == -1)
    {
        throw tcpsockerror( "socket" );
    }
    
    long yes { 0 };
    setsockopt( fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int) );
    if( bind( fd_, (struct sockaddr *)&b_addr, sizeof(b_addr)) == -1 )
    {
        close( fd_ );
        throw tcpsockerror( "bind" );
    }

    fcntl( fd_, F_SETFL, fcntl(fd_, F_GETFL, 0) | O_NONBLOCK );
    if( listen( fd_, SOMAXCONN ) == -1)
    {
        close( fd_ );
        throw tcpsockerror( "bind" );
    }
    std::cerr << "[*] listening on port " << ntohs(b_addr.sin_port) << "\n";
}

tcpsock::~tcpsock()
{
    close( fd_ );
    std::cerr << "[*] stop listening\n";
}
