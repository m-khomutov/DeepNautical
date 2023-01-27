/* 
 * File:   jpegdecoder.h
 * Author: mkh
 *
 * Created on 26 января 2023 г., 16:41
 */

#ifndef JPEGDECODER_H
#define JPEGDECODER_H

#include "basedecoder.h"

#include <cstddef>
#include <cstdio>
#include <cstdint>
extern "C"
{
 #include <jpeglib.h>
 #include <jerror.h>
}
#include <mutex>

class jpegdecoder: public basedecoder {
public:
    jpegdecoder();
    jpegdecoder(const jpegdecoder& orig) = delete;
    jpegdecoder &operator =(const jpegdecoder& orig) = delete;
    ~jpegdecoder();
private:
    jpeg_decompress_struct cinfo_;
    jpeg_error_mgr jerr_;
    std::vector< uint8_t > frame_[2];
    std::mutex mutex_;

private:
    void f_load( decframe *image ) override;
    void f_store( uint8_t const *frame, size_t size, uint32_t timestamp ) override;
};

#endif /* JPEGDECODER_H */

