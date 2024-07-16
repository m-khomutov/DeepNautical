/* 
 * File:   c_socket.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 10:34
 */

#include "flv.h"
#include "utils.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <cstring>
#include <iostream>

namespace
{
const char http_ok_status[] = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: video/x-flv\r\n\r\n";
const uint8_t flv_header[13] = { 'F', 'L', 'V', 0x01, 0x01, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x0, 0x00 };

template< size_t N = 3 >
void copyNbytes( uint32_t from, uint8_t *to )
{
    union
    {
        uint32_t value;
        uint8_t buf[sizeof(uint32_t)];
    } u;
    u.value = htobe32( from );   
    memcpy( to, u.buf + 1, 3 );

    if( N == 4 )
    {
        to[3] = u.buf[0];
    }
}
}  // namespace

flv::protocol::protocol( int b_sock, int flags, size_t view )
    : base::protocol( b_sock, flags )
    , http_flv_header_( strlen(http_ok_status) + sizeof(flv_header) )
{
    view_ = view;

    size_t http_status_size = http_flv_header_.size() - sizeof(flv_header);
    ::memcpy( http_flv_header_.data(), http_ok_status, http_status_size );
    ::memcpy( http_flv_header_.data() + http_status_size, flv_header, sizeof(flv_header) );
}

void flv::protocol::on_data( const uint8_t * data, int size )
{
    for( int i(0); i < size; ++i ) fprintf(stderr, "%c", char(data[i]));
    f_send_header();
}

void flv::protocol::do_write()
{
    if( sent_from_header_ != http_flv_header_.size() )
    {
        f_send_header();
    }
    if( sent_from_frame_ != flv_frame_.size() )
    {
        f_send_frame();
    }
}

void flv::protocol::send_frame( const uint8_t * data, int size )
{
    if( sent_from_header_ != http_flv_header_.size() )
    {
        f_send_header();
    }
    if( sent_from_header_ == http_flv_header_.size() )
    {
        if( sent_from_frame_ != flv_frame_.size() )
        {
            f_send_frame();
        }
        if( sent_from_frame_ == flv_frame_.size() )
        {
            if( !timestamp_ )
            {
                timestamp_ = utils::usec();
            }
            copyNbytes( size + 1, tag_header_ + 1 );
            copyNbytes< 4 >( utils::usec() - timestamp_, tag_header_ + 4 );

            uint32_t tagsize = htobe32( size + sizeof(tag_header_) );

            flv_frame_.resize( sizeof(tag_header_) + size + sizeof(tagsize) );
            ::memcpy( flv_frame_.data(), tag_header_, sizeof(tag_header_) );
            ::memcpy( flv_frame_.data() + sizeof(tag_header_), data, size );
            ::memcpy( flv_frame_.data() + sizeof(tag_header_) + size, &tagsize, sizeof(tagsize) );

            sent_from_frame_ = 0;
            f_send_frame();
        }
    }
}

bool flv::protocol::can_send_frame() const
{
    return true;
}

void flv::protocol::write_error()
{
    ::send( fd_, base::protocol::status_404, strlen(base::protocol::status_404), flags_ );
}

void flv::protocol::f_send_header()
{
    int rc = ::send( fd_, http_flv_header_.data() + sent_from_header_, http_flv_header_.size() - sent_from_header_, flags_ );
    if( rc > 0 )
    {
        sent_from_header_ += rc;
    }
}

void flv::protocol::f_send_frame()
{
    int rc = ::send( fd_, flv_frame_.data() + sent_from_frame_, flv_frame_.size() - sent_from_frame_, flags_ );
    if( rc > 0 )
    {
        sent_from_frame_ += rc;
    }
}
