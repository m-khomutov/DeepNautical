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

class c_socket_error: public std::runtime_error
{
public:
    c_socket_error( const std::string & what );
};

class c_socket {
public:
    c_socket( std::string const &addr, uint16_t port );
    c_socket(const c_socket& orig) = delete;
    c_socket &operator =(const c_socket& orig) = delete;
    ~c_socket();

    operator int() const
    {
        return fd_;
    }
    
    size_t send( uint8_t const *data, size_t size );
    size_t receive( uint8_t *buffer, size_t size );
    
private:
    int fd_;

private:
    in_addr_t f_resolve_host( std::string const &addr );
};

#endif /* C_SOCKET_H */

