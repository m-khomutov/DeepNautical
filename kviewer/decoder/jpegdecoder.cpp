/* 
 * File:   jpegdecoder.cpp
 * Author: mkh
 * 
 * Created on 26 января 2023 г., 16:41
 */

#include "jpegdecoder.h"

namespace
{
template< typename T >
void copyimage( const T from, T *to )
{
    if( to->size() != from.size() )
    {
        to->resize( from.size() );
    }
    to->assign( from.begin(), from.end() );   
}
}  // namespace

void jpegdecoder::f_load( NUtils::TImage *img )
{
    {
        std::lock_guard< std::mutex > lk( mutex_ );
        
        if( frame_[0].empty() || img->timestamp == timestamp_ )
        {
            throw basedecoder_nodata();
        }
        copyimage( frame_[0], &frame_[1] );
        img->timestamp = timestamp_;
    }
    codec_.decode( frame_[1].data(), frame_[1].size(), img );
}

void jpegdecoder::f_store( uint8_t const *frame, size_t size, uint64_t timestamp )
{
    std::lock_guard< std::mutex > lk( mutex_ );
    if( frame_[0].size() != size )
    {
        frame_[0].resize( size );
    }
    frame_[0].assign( frame, frame + size );
    timestamp_ = timestamp;
}
