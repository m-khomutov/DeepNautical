/* 
 * File:   jpegframe.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 16:40
 */

#include "jpegframe.h"
#include "protocol/baseprotocol.h"
#include <unistd.h>

namespace  // Содержит стафф, связанный с определение контекста сжатия кадра в JPEG
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

TJpegframe::TJpegframe( const NUtils::TGeometry &geometry, int quality, int duration, bool reverse )
: TBaseframe( geometry, duration )
, reverse_( reverse )
{
    // сам контекст сжатия
    cinfo_.err = jpeg_std_error( &jerr_ );  // errors get written to stderr 
    jpeg_create_compress( &cinfo_ );

    cinfo_.image_width = geometry_.width;
    cinfo_.image_height = geometry_.height;
    cinfo_.input_components = NUtils::TImage::EColor::RGB;
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

TJpegframe::~TJpegframe()
{
    jpeg_destroy_compress( &cinfo_ );
}

uint8_t *TJpegframe::buffer( size_t view, int width, int height )
{
    std::vector< uint8_t >::size_type sz = NUtils::TImage::EColor::RGB * width * height;

    if( view >= rgb_buffers_.size() ) // выделить место под буфер точки обзора
    {
        rgb_buffers_.resize( view + 1 );
        jpeg_frames_.resize( view + 1 );
    }

    if( sz != rgb_buffers_[view].size() )  // выделить память в буфере данных точки обзора и сохранить размеры кадра
    {
        rgb_buffers_[view].resize( sz );

        jpeg_frames_[view].frame.resize( sz );
        jpeg_frames_[view].geometry.width = width;
        jpeg_frames_[view].geometry.height = height;
    }

    return rgb_buffers_[view].data();  // сюда скопируется RGB-буфер
}

void TJpegframe::prepare_buffer( size_t view )
{
    //сжать данные в формат JPEG
    f_compress( view );
}

void TJpegframe::f_compress( size_t view )
{
    // сжать буфер точки обзора view
    mem_destination_ptr_t dest = mem_destination_ptr_t( cinfo_.dest );
    dest->buf = jpeg_frames_[view].frame.data();   // сюда скопируются данные в формате JPEG
    dest->bufsize  = jpeg_frames_[view].frame.size();
    dest->jpegsize = 0;

    cinfo_.image_width = jpeg_frames_[view].geometry.width;
    cinfo_.image_height = jpeg_frames_[view].geometry.height;
    cinfo_.input_components = NUtils::TImage::EColor::RGB;
    cinfo_.dct_method = JDCT_FASTEST;

    jpeg_start_compress( &cinfo_, TRUE );

    int stride = cinfo_.image_width * cinfo_.input_components;
    JSAMPROW row_ptr[1];
    uint8_t * data = rgb_buffers_[view].data();
    {
        if( reverse_ )
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
        jpeg_frames_[view].size_ = dest->jpegsize; // размер JPEG буфера
    }
}

bool TJpegframe::f_send_buffer( TBaseprotocol * proto )
{
    if( !proto || jpeg_frames_.empty() )
    {
        return false;
    }

    size_t view = proto->view();

    if( view > rgb_buffers_.size() - 1 )
    {
        return false;
    }

    if( jpeg_frames_[view].size_ )
    {
        // отправить абоненту
        proto->send_frame( jpeg_frames_[view].frame.data(), jpeg_frames_[view].size_ );
    }
    return true;
}
