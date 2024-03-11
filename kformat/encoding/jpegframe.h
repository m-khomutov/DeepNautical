/* 
 * File:   frame.h
 * Author: mkh
 *
 * Created on 24 января 2023 г., 16:40
 */

#ifndef JPEGFRAME_H
#define JPEGFRAME_H

#include "baseframe.h"

extern "C"
{
 #include <jpeglib.h>
 #include <jerror.h>
}

#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <vector>

class jpegframe: public baseframe {
public:
    jpegframe( const utils::geometry &geometry, int quality, int duration, bool reverse = true );
    jpegframe( const jpegframe& orig ) = delete;
    jpegframe &operator =( const jpegframe &rhs ) = delete;
    ~jpegframe();

    uint8_t *buffer( int width, int height ) override;

private:
    jpeg_compress_struct cinfo_;
    jpeg_error_mgr jerr_;

    std::vector< uint8_t > jpeg_frame_;
    size_t size_ { 0 };
    bool reverse_;

private:
    void f_load( baseprotocol * proto, float duration ) override;

    void f_compress();
};

#endif /* JPEGFRAME_H */
