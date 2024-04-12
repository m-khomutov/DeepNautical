/* 
 * File:   socket.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 9:24
 */

#include "s_socket.h"
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>

TSsocketError::TSsocketError( const std::string &what )
: std::runtime_error( what + std::string(" failed: ") + std::string(strerror( errno )) )
{}


TSsocket::TSsocket( uint16_t b_port )
{
    // открываем сокет на порту
    sockaddr_in b_addr;
    memset( &b_addr, 0, sizeof(struct sockaddr_in) );
    b_addr.sin_family = AF_INET;
    b_addr.sin_addr.s_addr = INADDR_ANY;
    b_addr.sin_port = htons(b_port);
    if( (fd_ = socket( b_addr.sin_family, SOCK_STREAM, 0 )) == -1)
    {
        throw TSsocketError( "socket" );
    }
    
    long yes { 0 };
    // объявляем его многоразовым
    setsockopt( fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int) );
    // привязывемся к адресу как сервер
    if( bind( fd_, (struct sockaddr *)&b_addr, sizeof(b_addr)) == -1 )
    {
        close( fd_ );
        throw TSsocketError( "bind" );
    }
    // делаем его неблокирующим
    fcntl( fd_, F_SETFL, fcntl(fd_, F_GETFL, 0) | O_NONBLOCK );
    // включаем прием запросов на соединение
    if( listen( fd_, SOMAXCONN ) == -1)
    {
        close( fd_ );
        throw TSsocketError( "bind" );
    }
    std::cerr << "[*] listening on port " << ntohs(b_addr.sin_port) << "\n";
}

TSsocket::~TSsocket()
{
    // RTTI
    close( fd_ );
    std::cerr << "[*] stop listening\n";
}
