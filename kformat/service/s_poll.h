/* 
 * File:   s_poll.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 10:06
 */

#ifndef S_POLL_H
#define S_POLL_H

#include "s_socket.h"
#include "videodevice.h"
#include "../encoding/baseframe.h"
#include <atomic>
#include <chrono>
#include <memory>
#include <map>

class basescreen;
class connection;

class s_poll_error: public std::runtime_error
{
public:
    s_poll_error( const std::string & what );
};

class s_poll {
public:
    s_poll( basescreen *screen, uint16_t port );
    s_poll( char const *videodevname, uint16_t port );
    s_poll(const s_poll& orig) = delete;
    s_poll &operator =(const s_poll& orig) = delete;
    ~s_poll();

    void run();
    void stop();

private:
    enum { maxevents = 32 };
    
    std::atomic< bool > running_ { true };
    s_socket p_socket_;
    int fd_;
    basescreen *screen_ { nullptr };
    std::unique_ptr< videodevice > videodev_;
    std::map< int, std::shared_ptr< connection > > connections_;

private:
    void f_add( int sock, uint32_t events );
    void f_send_frame( TBaseframe::time_point_t *last_ts );
};

#endif /* S_POLL_H */
