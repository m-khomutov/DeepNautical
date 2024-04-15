/* 
 * File:   c_socket.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 18:04
 */

#ifndef C_SOCKET_H
#define C_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <string>
#include <stdexcept>

class TCsocketError: public std::runtime_error
{
public:
    TCsocketError( const std::string & what );
};

class TCsocket {
public:
    TCsocket( std::string const &addr, uint16_t port );
    TCsocket( const TCsocket& orig) = delete;
    TCsocket &operator =( const TCsocket& orig ) = delete;
    ~TCsocket();

    operator int() const
    {
        return fd_;
    }
    
    ssize_t send( uint8_t const *data, size_t size );
    ssize_t receive( uint8_t *buffer, size_t size );
    
private:
    int fd_;

private:
    in_addr_t f_resolve_host( std::string const &addr );
};

#endif /* C_SOCKET_H */

