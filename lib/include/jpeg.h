/*!
     \file jpegframe.h
     \author mkh
     \date 24 января 2023 г.
     \brief Заголовочный файл класса представления видеокадра в формате JPEG.

     Данный файл содержит в себе состояние класса представления видеокадра в формате JPEG, объявление его интерфейсов.
 */

#ifndef JPEGFRAME_H
#define JPEGFRAME_H

#include "frame.h"

extern "C"
{
 #include <jpeglib.h>
 #include <jerror.h>
}

#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <vector>

namespace jpeg {

class frame: public base::frame
{
public:
    frame( const utils::winsize &sz, int quality, int duration, bool mirrored = true );
    frame( const frame& other ) = delete;
    ~frame();

    frame &operator =( const frame &other ) = delete;

    uint8_t *buffer( size_t view, int width, int height ) override;
    void prepare_buffer( size_t view ) override;

private:
    struct image
    {
        base::frame::pixels frame;
        utils::winsize size;
        size_t framesize { 0 };
    };

    jpeg_compress_struct cinfo_;
    jpeg_error_mgr jerr_;

    std::vector< image > jpeg_images_;
    bool mirrored_ { false };

private:
    bool f_send_buffer( base::protocol * p ) override;
    void f_compress( size_t view );
};

}  // namespace jpeg
#endif /* JPEGFRAME_H */
