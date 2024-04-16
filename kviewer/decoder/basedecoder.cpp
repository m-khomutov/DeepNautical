/* 
 * File:   basedecoder.cpp
 * Author: mkh
 * 
 * Created on 26 января 2023 г., 16:40
 */

#include "basedecoder.h"


TBasedecoderError::TBasedecoderError( const std::string &what )
: std::runtime_error( what )
{}

TBasedecoderNodata::TBasedecoderNodata()
: std::runtime_error( "" )
{}

bool TBasedecoder::copy_frame( NUtils::TImage *img )
{
    try
    {
        f_copy_frame( img );
    }
    catch( TBasedecoderNodata const & e )
    {
        return false;
    }
    return true;
}

void TBasedecoder::save_frame( uint8_t const *frame, size_t size, uint64_t timestamp )
{
    f_save_frame( frame, size, timestamp );
}
