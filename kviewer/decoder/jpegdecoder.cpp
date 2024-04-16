/* 
 * File:   jpegdecoder.cpp
 * Author: mkh
 * 
 * Created on 26 января 2023 г., 16:41
 */

#include "jpegdecoder.h"

// потокобезопасно копирует принятый фрейм в буфер обработки и декодирует его там
void TJpegdecoder::f_copy_frame( NUtils::TImage *img )
{
    {
        std::lock_guard< std::mutex > lk( mutex_ );
        
        if( frame_[0].empty() || img->timestamp == timestamp_ )
        {
            throw TBasedecoderNodata();
        }
        //  cохранить - своп данных и временную метку
        std::swap( frame_[0], frame_[1] );
        img->timestamp = timestamp_;
    }
    // декодируем из JPEG-a
    codec_.decode( frame_[1].data(), frame_[1].size(), img );
}

// потокобезопасно сохраняет кадр данных, принятый из сети
void TJpegdecoder::f_save_frame( uint8_t const *frame, size_t size, uint64_t timestamp )
{
    std::lock_guard< std::mutex > lk( mutex_ );
    if( frame_[0].size() != size )
    {
        frame_[0].resize( size );
    }
    frame_[0].assign( frame, frame + size );
    timestamp_ = timestamp;
}
