/* 
 * File:   s_poll.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 10:06
 */

#ifndef S_POLL_H
#define S_POLL_H

#include "s_socket.h"
#include <atomic>
#include <chrono>
#include <memory>
#include <map>

class baseframe;
class baseprotocol;

class s_poll_error: public std::runtime_error
{
public:
    s_poll_error( const std::string & what );
};

class s_poll {
public:
    explicit s_poll( baseframe *frame );
    s_poll(const s_poll& orig) = delete;
    s_poll &operator =(const s_poll& orig) = delete;
    ~s_poll();

    void run();
    void stop();

private:
    enum { maxevents = 32 };
    using time_point_t = std::chrono::time_point< std::chrono::high_resolution_clock >;
    
    std::atomic< bool > running_ { true };
    s_socket p_socket_;
    int fd_;
    baseframe *frame_;
    std::chrono::milliseconds frame_duration_;
    std::map< int, std::shared_ptr< baseprotocol > > connections_;

private:
    void f_add( int sock, uint32_t events );
    void f_send_frame( time_point_t *last_ts );
};

#endif /* S_POLL_H */
