#include "mjpegprotocol.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <cstring>

namespace
{
const char ok_status[] = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: multipart/x-mixed-replace;boundary=agat-a_mjpeg\r\n\r\n";
const char boundary[] = "--agat-a_mjpeg\r\nContent-Type: image/jpeg\r\nContent-Length: ";
}

TMjpegprotocol::TMjpegprotocol( int b_sock, int flags, const std::string &scene )
: TBaseprotocol( b_sock, flags )
, boundary_size_( strlen(boundary) )
{
    scene_ = scene;
}

void TMjpegprotocol::on_data( const uint8_t * data, int size )
{
    for( int i(0); i < size; ++i ) fprintf(stderr, "%c", char(data[i]));
}

void TMjpegprotocol::do_write()
{
    if( sent_ != mjpeg_frame_.size() )
    {
        f_send_frame();
    }
}

void TMjpegprotocol::send_frame( const uint8_t * data, int data_size )
{
    // размер кадра для Content-Length. Без этого поля ffmpeg не алё, хотя vlc и firefox вполне
    char content_length[64];
    size_t content_length_size = sprintf( content_length, "%u\r\n\r\n", data_size );

    size_t header_size = header_sent_ ? 0 : strlen(ok_status); // добавляем заголовок http для первого кадра
    size_t message_size = header_size + boundary_size_ + content_length_size + data_size; // полный отправляемый размер

    mjpeg_frame_.resize( message_size );

    off_t off = 0;
    // при первом кадре еще добавить заголовок http
    if( header_size )
    {
        ::memcpy( mjpeg_frame_.data(), ok_status, header_size );
        off += header_size;
        header_sent_ = true;
    }
    // разделитель
    ::memcpy( mjpeg_frame_.data() + off, boundary, boundary_size_ );
    off += boundary_size_;
    // размер кадра и конец заголовков
    ::memcpy( mjpeg_frame_.data() + off, content_length, content_length_size );
    off += content_length_size;
    // кадр
    ::memcpy( mjpeg_frame_.data() + off, data, data_size );

    sent_ = 0;
    f_send_frame();
}

bool TMjpegprotocol::can_send_frame() const
{
    return true;
}

void TMjpegprotocol::write_error()
{
    ::send( fd_, TBaseprotocol::status_404, strlen(TBaseprotocol::status_404), flags_ );
}

void TMjpegprotocol::f_send_frame()
{
    int rc = ::send( fd_, mjpeg_frame_.data() + sent_, mjpeg_frame_.size() - sent_, flags_ );
    if( rc > 0 )
    {
        sent_ += rc;
    }
}
