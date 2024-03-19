/* 
 * File:   c_socket.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 10:34
 */

#include "flvprotocol.h"
#include "../utils.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <cstring>
#include <iostream>

namespace
{
const char status[] = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: video/x-flv\r\n\r\n";
const uint8_t header[13] = { 'F', 'L', 'V', 0x01, 0x01, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x0, 0x00 };

void copy3bytes( uint32_t from, uint8_t *to )
{
    union
    {
        uint32_t value;
        uint8_t buf[sizeof(uint32_t)];
    } u;
    u.value = htobe32( from );   
    memcpy( to, u.buf + 1, 3 );
}
}  // namespace

flvprotocol::flvprotocol( int b_sock, int flags )
: baseprotocol( b_sock, flags )
, http_flv_header_( strlen(status) + 13 )
{
    ::memcpy( http_flv_header_.data(), status, strlen(status) );
    ::memcpy( http_flv_header_.data() + http_flv_header_.size() - 13, header, sizeof(header) );
}

flvprotocol::~flvprotocol()
{
}

void flvprotocol::on_data( const uint8_t * data, int size )
{
    for( int i(0); i < size; ++i ) fprintf(stderr, "%c", char(data[i]));
    f_send_header();
}

void flvprotocol::do_write()
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

void flvprotocol::send_frame( const uint8_t * data, int size, float )
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
            copy3bytes( size + 1, tag_header_ + 1 );
            //copy3bytes( timestamp_, tag_header_ + 4 );
            //tag_header_[7] = (timestamp_ >> 24) & 0xff;
            timestamp_ = htobe64(utils::now());
            memcpy( tag_header_ + 4, &timestamp_, sizeof(timestamp_) );
    
            //timestamp_ += duration;

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

bool flvprotocol::can_send_frame() const
{
    return true;
}

void flvprotocol::f_send_header()
{
    int rc = ::send( fd_, http_flv_header_.data() + sent_from_header_, http_flv_header_.size() - sent_from_header_, flags_ );
    if( rc > 0 )
    {
        sent_from_header_ += rc;
    }
}

void flvprotocol::f_send_frame()
{
    int rc = ::send( fd_, flv_frame_.data() + sent_from_frame_, flv_frame_.size() - sent_from_frame_, flags_ );
    if( rc > 0 )
    {
        sent_from_frame_ += rc;
    }
}
