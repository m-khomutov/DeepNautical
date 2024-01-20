/* 
 * File:   connection.h
 * Author: mkh
 *
 * Created on 2 марта 2023 г., 13:29
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <kformat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <memory>

class basescreen;

class connection {
public:
    explicit connection( int b_sock );
    connection(const connection& orig) = delete;
    connection &operator =(const connection& orig) = delete;
    ~connection();

    operator int() const {
        return fd_;
    }

    baseprotocol * protocol()
    {
        return proto_.get();
    }

    void on_data( basescreen* screen, const uint8_t * data, int size );
    void on_ready_to_write();
    void send_frame( const uint8_t * data, int size, float duration );

private:
    socklen_t socklen_ { sizeof(sockaddr_in) };
    int fd_;
    sockaddr_in address_;
    std::unique_ptr< baseprotocol > proto_;
    std::string request_;

private:
    void f_determine_protocol( basescreen *screen, const uint8_t * data, int size );
};

#endif /* CONNECTION_H */

