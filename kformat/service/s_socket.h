/* 
 * File:   socket.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 9:24
 */

#ifndef S_SOCKET_H
#define S_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdexcept>

class s_socket_error: public std::runtime_error
{
public:
    s_socket_error( const std::string & what );
};

class s_socket {
public:
    explicit s_socket( uint16_t b_port );
    s_socket(const s_socket& orig) = delete;
    s_socket &operator =(const s_socket& orig) = delete;
    ~s_socket();

    operator int() const {
        return fd_;
    }
    
private:
    int fd_;

};

#endif /* S_SOCKET_H */
