/* 
 * File:   jpegdecoder.h
 * Author: mkh
 *
 * Created on 26 января 2023 г., 16:41
 */

#ifndef JPEGDECODER_H
#define JPEGDECODER_H

#include "basedecoder.h"

#include <mutex>

class jpegdecoder: public basedecoder {
public:
    jpegdecoder() = default;
    jpegdecoder(const jpegdecoder& orig) = delete;
    jpegdecoder &operator =(const jpegdecoder& orig) = delete;
    ~jpegdecoder() = default;

private:
    utils::jpeg_codec codec_;
    std::vector< uint8_t > frame_[2];
    std::mutex mutex_;

private:
    void f_load( utils::image *img ) override;
    void f_store( uint8_t const *frame, size_t size, uint64_t timestamp ) override;
};

#endif /* JPEGDECODER_H */
