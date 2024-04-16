/* 
 * File:   c_socket.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 10:34
 */

#include "flvprotocol.h"
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
const char http_not_found_status[] = "HTTP/1.1 404 Stream Not Found\r\nContent-Type: text/plain\r\nContent-Length: 0\r\n\r\n";

// копирует 3 байта в порядке big-endian
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

TFLVprotocol::TFLVprotocol( int b_sock, int flags, size_t view )
: TBaseprotocol( b_sock, flags )
, http_flv_header_( strlen(http_ok_status) + 13 )
, view_( view )
{
    ::memcpy( http_flv_header_.data(), http_ok_status, strlen(http_ok_status) );
    ::memcpy( http_flv_header_.data() + http_flv_header_.size() - 13, flv_header, sizeof(flv_header) );
}

TFLVprotocol::~TFLVprotocol()
{}

void TFLVprotocol::on_data( const uint8_t * data, int size )
{
    for( int i(0); i < size; ++i ) fprintf(stderr, "%c", char(data[i]));
    f_send_header();
}

void TFLVprotocol::do_write()
{
    // что не отослано полностью - дослать
    if( sent_from_header_ != http_flv_header_.size() )
    {
        f_send_header();
    }
    if( sent_from_frame_ != flv_frame_.size() )
    {
        f_send_frame();
    }
}

void TFLVprotocol::send_frame( const uint8_t * data, int size )
{
    // если заголовок не ушел полностью, отправить его (или что не ушло)
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
        // если прошлый фрейм ушел полностью, делаем новый
        if( sent_from_frame_ == flv_frame_.size() )
        {
            /* формат
               TagType   | UI8 = 9
               DataSize  | UI24
               Timestamp | UI64
               FrameType | UB[4] = 1
               CodecID   | UB[4] = 1
            */
            copy3bytes( size + 1, tag_header_ + 1 ); // DataSize
            /* Здесь отход от спецификации. Надо так (таймстамп в 3-х байтах как смещение от нуля)
               copy3bytes( timestamp_, tag_header_ + 4 );
               timestamp_ += duration;
             */
            timestamp_ = htobe64(NUtils::now());
            memcpy( tag_header_ + 4, &timestamp_, sizeof(timestamp_) );  // Timestamp

            // после фрейма идет его размер
            uint32_t tagsize = htobe32( size + sizeof(tag_header_) );

            // окончательно уходит: заголовок + кадр данных + размер(заголовок + данные)
            flv_frame_.resize( sizeof(tag_header_) + size + sizeof(tagsize) );
            ::memcpy( flv_frame_.data(), tag_header_, sizeof(tag_header_) );
            ::memcpy( flv_frame_.data() + sizeof(tag_header_), data, size );
            ::memcpy( flv_frame_.data() + sizeof(tag_header_) + size, &tagsize, sizeof(tagsize) );

            sent_from_frame_ = 0;
            f_send_frame();
        }
    }
}

bool TFLVprotocol::can_send_frame() const
{
    return true;
}

void TFLVprotocol::write_error()
{
    ::send( fd_, http_not_found_status, strlen(http_not_found_status), flags_ );
}

void TFLVprotocol::f_send_header()
{
    int rc = ::send( fd_, http_flv_header_.data() + sent_from_header_, http_flv_header_.size() - sent_from_header_, flags_ );
    if( rc > 0 )
    {
        sent_from_header_ += rc;
    }
}

void TFLVprotocol::f_send_frame()
{
    int rc = ::send( fd_, flv_frame_.data() + sent_from_frame_, flv_frame_.size() - sent_from_frame_, flags_ );
    if( rc > 0 )
    {
        sent_from_frame_ += rc;
    }
}
