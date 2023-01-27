/* 
 * File:   frame.h
 * Author: mkh
 *
 * Created on 24 января 2023 г., 16:40
 */

#ifndef JPEGFRAME_H
#define JPEGFRAME_H

#include "baseframe.h"

#include <cstddef>
#include <cstdio>
extern "C"
{
 #include <jpeglib.h>
 #include <jerror.h>
}

#include <cstdint>
#include <vector>
#include <mutex>

class jpegframe: public baseframe {
public:
    jpegframe( int width, int height, int quality );
    jpegframe( const jpegframe& orig ) = delete;
    jpegframe &operator =( const jpegframe &rhs ) = delete;
    ~jpegframe();

private:
    jpeg_compress_struct cinfo_;
    jpeg_error_mgr jerr_;

    std::vector< uint8_t > rgb_buffer_;
    std::vector< uint8_t > jpeg_frame_;
    size_t size_ { 0 };
    std::mutex mutex_;

private:
    void f_store( int width, int height ) override;
    void f_load( baseprotocol * proto, float duration ) override;
    
    void f_encode();

};

#endif /* JPEGFRAME_H */
