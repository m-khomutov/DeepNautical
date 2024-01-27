/* 
 * File:   frame.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 16:40
 */

#include "jpegframe.h"
#include "protocol/baseprotocol.h"
#include <unistd.h>

namespace
{
typedef struct
{
    jpeg_destination_mgr pub;
    JOCTET *buf;
    size_t bufsize;
    size_t jpegsize;
} mem_destination_mgr_t;

typedef mem_destination_mgr_t * mem_destination_ptr_t;

METHODDEF(void) init_destination(j_compress_ptr cinfo)
{
    mem_destination_ptr_t dest = mem_destination_ptr_t( cinfo->dest );
    dest->pub.next_output_byte = dest->buf;
    dest->pub.free_in_buffer = dest->bufsize;
    dest->jpegsize = 0;
}

METHODDEF(boolean) empty_output_buffer(j_compress_ptr cinfo)
{
    mem_destination_ptr_t dest = mem_destination_ptr_t( cinfo->dest );
    dest->pub.next_output_byte = dest->buf;
    dest->pub.free_in_buffer = dest->bufsize;
    return FALSE;
    ERREXIT(cinfo, JERR_BUFFER_SIZE);
}

METHODDEF(void) term_destination(j_compress_ptr cinfo)
{
    mem_destination_ptr_t dest = mem_destination_ptr_t( cinfo->dest );
    dest->jpegsize = dest->bufsize - dest->pub.free_in_buffer;
}

}  // namespace

jpegframe::jpegframe( const utils::geometry &geometry, int quality, int duration )
: baseframe( geometry, duration )
{
    cinfo_.err = jpeg_std_error( &jerr_ );  // errors get written to stderr 
    jpeg_create_compress( &cinfo_ );

    cinfo_.image_width = geometry_.width;
    cinfo_.image_height = geometry_.height;
    cinfo_.input_components = 3;
    cinfo_.in_color_space = JCS_RGB;
    jpeg_set_defaults( &cinfo_ );
    jpeg_set_quality( &cinfo_, quality, TRUE );
    cinfo_.dest =
            (jpeg_destination_mgr *)( *cinfo_.mem->alloc_small)( (j_common_ptr)&cinfo_,
                                      JPOOL_PERMANENT,
                                      sizeof(mem_destination_mgr_t) );
    mem_destination_ptr_t dest = mem_destination_ptr_t( cinfo_.dest );
    dest->pub.init_destination = init_destination;
    dest->pub.empty_output_buffer = empty_output_buffer;
    dest->pub.term_destination = term_destination;
}

jpegframe::~jpegframe()
{
    jpeg_destroy_compress( &cinfo_ );
}

uint8_t *jpegframe::buffer( int width, int height )
{
    size_t channels { 3 };
    size_t stride = channels * width;
    //stride += (stride % 4) ? (4 - stride % 4) : 0;
    std::vector< uint8_t >::size_type sz = stride * height;

    if( sz != rgb_buffer_.size() )
    {
        rgb_buffer_.resize( sz );
        jpeg_frame_.resize( sz );
        cinfo_.image_width = width;
        cinfo_.image_height = height;
        cinfo_.input_components = 3;
        cinfo_.dct_method = JDCT_FASTEST;
    }

    return rgb_buffer_.data();
}

void jpegframe::f_compress()
{
    mem_destination_ptr_t dest = mem_destination_ptr_t( cinfo_.dest );
    dest->buf = jpeg_frame_.data();
    dest->bufsize  = jpeg_frame_.size();
    dest->jpegsize = 0;

    jpeg_start_compress( &cinfo_, TRUE );

    int stride = cinfo_.image_width * cinfo_.input_components;
    JSAMPROW row_ptr[1];
    uint8_t * data = rgb_buffer_.data();
    {
        for( int y(cinfo_.image_height - 1); y >= 0; --y )
        {
            row_ptr[0] = &data[y * stride];
            jpeg_write_scanlines( &cinfo_, row_ptr, 1 );
        }
        jpeg_finish_compress( &cinfo_ );
        size_ = dest->jpegsize;
    }
}

void jpegframe::f_load( baseprotocol * proto, float duration )
{
    f_compress();

    if( size_ )
    {
        proto->send_frame( jpeg_frame_.data(), size_, duration );
    }
}
