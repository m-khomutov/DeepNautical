/*!
     \file connection.h
     \author mkh
     \date 2 марта 2023 г.
     \brief Заголовочный файл класса сетевого соединения с абонентом.

     Данный файл содержит в себе состояние класса сетевого соединения с абонентом, объявление его интерфейсов.
 */
#ifndef CONNECTION_H
#define CONNECTION_H

#include "protocol.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <memory>

namespace base
{

class screen;

}

class connection {
public:
    explicit connection( int b_sock );
    connection( const connection& other ) = delete;
    ~connection();

    connection &operator =( const connection& other ) = delete;

    operator int() const
    {
        return fd_;
    }

    base::protocol * protocol()
    {
        return proto_.get();
    }

    void on_data( base::screen *screen, const uint8_t * data, int size );
    void on_ready_to_write();

private:
    socklen_t socklen_ { sizeof(sockaddr_in) };
    int fd_;
    int send_flags_ = MSG_ZEROCOPY;
    sockaddr_in address_;
    std::unique_ptr< base::protocol > proto_;
    std::string request_;
};

#endif /* CONNECTION_H */
