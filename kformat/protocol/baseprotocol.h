/* 
 * File:   baseprotocol.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 14:59
 */

#ifndef BASEPROTOCOL_H
#define BASEPROTOCOL_H

#include <stdexcept>

class protocol_error: public std::runtime_error
{
public:
    protocol_error( const std::string & what );
};

class basescreen;

class baseprotocol {
public:
    static baseprotocol * create( basescreen *screen, const std::string &request, int sock, int flags );

    explicit baseprotocol( int sock, int flags );
    baseprotocol(const baseprotocol& orig) = delete;
    baseprotocol &operator =(const baseprotocol& orig) = delete;
    virtual ~baseprotocol();

    virtual void on_data( const uint8_t * data, int size ) = 0;
    virtual void do_write() = 0;
    virtual void send_frame( const uint8_t * data, int size, float duration ) = 0;
    virtual bool can_send_frame() const = 0;
    
protected:
    int fd_;
    int flags_;

private:

};

#endif /* BASEPROTOCOL_H */

