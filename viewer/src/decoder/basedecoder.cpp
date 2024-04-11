/* 
 * File:   basedecoder.cpp
 * Author: mkh
 * 
 * Created on 26 января 2023 г., 16:40
 */

#include "basedecoder.h"
#include <iostream>


basedecoder_error::basedecoder_error( const std::string &what )
: std::runtime_error( what )
{
}

basedecoder_nodata::basedecoder_nodata()
: std::runtime_error( "" )
{
}

basedecoder::basedecoder()
{
}

basedecoder::~basedecoder()
{
}


bool basedecoder::load( NUtils::TImage *img )
{
    try
    {
        f_load( img );
    }
    catch( basedecoder_error const & e )
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    catch( basedecoder_nodata const & e )
    {
        return false;
    }
    return true;
}

void basedecoder::store( uint8_t const *frame, size_t size, uint64_t timestamp )
{
    f_store( frame, size, timestamp );
}
