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

jpegdecoder::jpegdecoder()
{
    cinfo_.err = jpeg_std_error( &jerr_ );	
    jpeg_create_decompress( &cinfo_ );
}

jpegdecoder::~jpegdecoder()
{
    jpeg_destroy_decompress( &cinfo_ );
}

void jpegdecoder::f_load( decframe *image )
{
    {
        std::lock_guard< std::mutex > lk( mutex_ );
        
        if( frame_[0].empty() || image->timestamp == timestamp_ )
        {
            throw basedecoder_nodata();
        }
        copyimage( frame_[0], &frame_[1] );
        image->timestamp = timestamp_;
    }
    
    jpeg_mem_src( &cinfo_, frame_[1].data(), frame_[1].size() );
    if( jpeg_read_header( &cinfo_, TRUE ) != 1 )
    {
        throw basedecoder_error( "invalid JPEG" );
    }
    jpeg_start_decompress( &cinfo_ );

    image->window.width = cinfo_.output_width;
    image->window.height = cinfo_.output_height;
    image->channels = cinfo_.output_components;
    int row_stride = image->window.width * image->channels;
    
    size_t size = row_stride * image->window.height;
    if( image->pixels.size() != size )
    {
        image->pixels.resize( size );
    }

    while( cinfo_.output_scanline < cinfo_.output_height )
    {
        uint8_t *b_array[1] = { image->pixels.data() + cinfo_.output_scanline * row_stride };
        jpeg_read_scanlines( &cinfo_, b_array, 1 );
    }
    jpeg_finish_decompress( &cinfo_ );
}

void jpegdecoder::f_store( uint8_t const *frame, size_t size, uint32_t timestamp )
{
    std::lock_guard< std::mutex > lk( mutex_ );
    if( frame_[0].size() != size )
    {
        frame_[0].resize( size );
    }
    frame_[0].assign( frame, frame + size );
    timestamp_ = timestamp;
}
