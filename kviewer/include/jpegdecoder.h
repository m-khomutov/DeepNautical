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

class jpegdecoder: public TBasedecoder {
public:
    jpegdecoder() = default;
    jpegdecoder(const jpegdecoder& orig) = delete;
    jpegdecoder &operator =(const jpegdecoder& orig) = delete;
    ~jpegdecoder() = default;

private:
    NUtils::TJpegCodec codec_;
    std::vector< uint8_t > frame_[2];
    std::mutex mutex_;

private:
    void f_copy_frame( NUtils::TImage *img ) override;
    void f_save_frame( uint8_t const *frame, size_t size, uint64_t timestamp ) override;
};

#endif /* JPEGDECODER_H */
