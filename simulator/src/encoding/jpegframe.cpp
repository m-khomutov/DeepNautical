/* 
 * File:   frame.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 16:40
 */

#include "jpegframe.h"
#include "../server/baseprotocol.h"
#include <GL/glew.h>
#include <unistd.h>
#include <iostream>

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

jpegframe::jpegframe()
{
    cinfo_.err = jpeg_std_error( &jerr_ );  // errors get written to stderr 
    jpeg_create_compress( &cinfo_ );

    cinfo_.image_width = geometry_.width;
    cinfo_.image_height = geometry_.height;
    cinfo_.input_components = 3;
    cinfo_.in_color_space = JCS_RGB;
    jpeg_set_defaults( &cinfo_ );
    jpeg_set_quality( &cinfo_, utils::config()["quality"], TRUE );
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

void jpegframe::f_store( int width, int height )
{
    GLsizei channels { 3 };
    GLsizei stride = channels * width;
    //stride += (stride % 4) ? (4 - stride % 4) : 0;
    GLsizei sz = stride * height;
 
    if( sz != rgb_buffer_.size() )
    {
        rgb_buffer_.resize( sz );
        jpeg_frame_.resize( sz );
    	cinfo_.image_width = width;
    	cinfo_.image_height = height;
    	cinfo_.input_components = 3;
    	cinfo_.dct_method = JDCT_FASTEST;
    }
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    glReadBuffer( GL_FRONT );
    glReadPixels( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, rgb_buffer_.data() );
    
    f_encode();
}

void jpegframe::f_load( baseprotocol * proto, float duration )
{
    std::lock_guard< std::mutex > lk( mutex_ );
    if( size_)
    {
        proto->send_frame( jpeg_frame_.data(), size_, duration );   
    }
}

void jpegframe::f_encode()
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
        std::lock_guard< std::mutex > lk( mutex_ );
        for( int y(cinfo_.image_height - 1); y >= 0; --y )
        {
            row_ptr[0] = &data[y * stride];
            jpeg_write_scanlines( &cinfo_, row_ptr, 1 );
        }
        jpeg_finish_compress( &cinfo_ );
        size_ = dest->jpegsize;
    }
}
