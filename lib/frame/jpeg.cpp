/* 
 * File:   jpegframe.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 16:40
 */

#include "jpeg.h"
#include "protocol.h"
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

jpeg::frame::frame( const utils::winsize &sz, int quality, int duration, bool mirrored )
    : base::frame( sz, duration )
    , mirrored_( mirrored )
{
    cinfo_.err = jpeg_std_error( &jerr_ );
    jpeg_create_compress( &cinfo_ );

    cinfo_.image_width = size_.width;
    cinfo_.image_height = size_.height;
    cinfo_.input_components = utils::frame::colormode::RGB;
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

jpeg::frame::~frame()
{
    jpeg_destroy_compress( &cinfo_ );
}

uint8_t *jpeg::frame::buffer( size_t view, int width, int height )
{
    std::vector< uint8_t >::size_type sz = utils::frame::colormode::RGB * width * height;

    if( view >= rgb_.size() )
    {
        rgb_.resize( view + 1 );
        jpeg_images_.resize( view + 1 );
    }

    if( sz != rgb_[view].size() )
    {
        rgb_[view].resize( sz );

        jpeg_images_[view].frame.resize( sz );
        jpeg_images_[view].size.width = width;
        jpeg_images_[view].size.height = height;
    }

    return rgb_[view].data();
}

void jpeg::frame::prepare_buffer( size_t view )
{
    f_compress( view );
}

void jpeg::frame::f_compress( size_t view )
{
    mem_destination_ptr_t dest = mem_destination_ptr_t( cinfo_.dest );
    dest->buf = jpeg_images_[view].frame.data();
    dest->bufsize  = jpeg_images_[view].frame.size();
    dest->jpegsize = 0;

    cinfo_.image_width = jpeg_images_[view].size.width;
    cinfo_.image_height = jpeg_images_[view].size.height;
    cinfo_.input_components = utils::frame::colormode::RGB;
    cinfo_.dct_method = JDCT_FASTEST;

    jpeg_start_compress( &cinfo_, TRUE );

    int stride = cinfo_.image_width * cinfo_.input_components;
    JSAMPROW row_ptr[1];
    uint8_t * data = rgb_[view].data();
    {
        if( mirrored_ )
        {
            for( int y(cinfo_.image_height - 1); y >= 0; --y )
            {
                row_ptr[0] = &data[y * stride];
                jpeg_write_scanlines( &cinfo_, row_ptr, 1 );
            }
        }
        else
        {
            for( int y(0); y < int(cinfo_.image_height); ++y )
            {
                row_ptr[0] = &data[y * stride];
                jpeg_write_scanlines( &cinfo_, row_ptr, 1 );
            }
        }
        jpeg_finish_compress( &cinfo_ );
        jpeg_images_[view].framesize = dest->jpegsize;
    }
}

bool jpeg::frame::f_send_buffer( base::protocol * pr )
{
    if( !pr || jpeg_images_.empty() )
    {
        return false;
    }

    size_t view = pr->view();

    if( view > rgb_.size() - 1 )
    {
        return false;
    }

    if( jpeg_images_[view].framesize )
    {
        pr->send_frame( jpeg_images_[view].frame.data(), jpeg_images_[view].framesize );
    }
    return true;
}
