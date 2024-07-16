/*!
     \file s_socket.h
     \author mkh
     \date 25 января 2023 г.
     \brief Заголовочный файл класса сокета сетевого соединения с абонентом.

     Данный файл содержит в себе состояние класса сокета сетевого соединения с абонентом, объявление его интерфейсов.
 */
#ifndef S_SOCKET_H
#define S_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdexcept>

class tcpsockerror: public std::runtime_error
{
public:
    tcpsockerror( const std::string & what );
};

class tcpsock
{
public:
    explicit tcpsock( uint16_t b_port );
    tcpsock( const tcpsock& other ) = delete;
    ~tcpsock();

    tcpsock &operator =( const tcpsock& other ) = delete;
    operator int() const
    {
        return fd_;
    }
    
private:
    int fd_;

};

#endif /* S_SOCKET_H */
