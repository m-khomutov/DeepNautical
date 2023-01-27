/* 
 * File:   baseprotocol.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 14:59
 */

#ifndef BASEPROTOCOL_H
#define BASEPROTOCOL_H

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <stdexcept>

class protocol_error: public std::runtime_error
{
public:
    protocol_error( const std::string & what );
};

class baseprotocol {
public:
    explicit baseprotocol( int b_sock );
    baseprotocol(const baseprotocol& orig) = delete;
    baseprotocol &operator =(const baseprotocol& orig) = delete;
    virtual ~baseprotocol();

    operator int() const {
        return fd_;
    }

    virtual void on_data( const uint8_t * data, int size ) = 0;
    virtual void send_frame( const uint8_t * data, int size, float duration ) = 0;
    
protected:
    socklen_t socklen_ { sizeof(sockaddr_in) };
    int fd_;
    sockaddr_in address_;

private:

};

#endif /* BASEPROTOCOL_H */

