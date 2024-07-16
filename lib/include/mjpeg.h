/*!
     \file mjpegprotocol.h
     \author mkh
     \date 18 апреля 2024 г.
     \brief Заголовочный файл класса сетевого протокола формата MJPEG (Motion JPEG).

     Данный файл содержит в себе состояние класса сетевого протокола формата MJPEG (Motion JPEG), реализацию его интерфейсов.
 */
#ifndef MJPEGPROTOCOL_H
#define MJPEGPROTOCOL_H

#include "protocol.h"

namespace mjpeg
{

class protocol: public base::protocol
{
public:
    protocol(  int b_sock, int flags, size_t view );
    protocol( const protocol& other ) = delete;
    ~protocol() = default;

    protocol & operator =( const protocol& other ) = delete;

    void on_data( const uint8_t * data, int size ) override;
    void do_write() override;
    void send_frame( const uint8_t * data, int size ) override;
    bool can_send_frame() const override;
    void write_error() override;

private:
    std::vector< uint8_t > mjpeg_frame_;
    size_t boundary_size_;
    bool header_sent_ {false};
    size_t sent_ { 0 };

private:
    void f_send_frame();
};

}  // namespace mjpeg
#endif // MJPEGPROTOCOL_H
