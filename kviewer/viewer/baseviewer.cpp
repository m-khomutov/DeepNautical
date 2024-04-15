/* 
 * File:   baseviewer.cpp
 * Author: mkh
 * 
 * Created on 6 февраля 2023 г., 16:09
 */

#include "baseviewer.h"
#include "decoder/jpegdecoder.h"


viewer_error::viewer_error( const std::string &what )
: std::runtime_error( what )
{
}

baseviewer::baseviewer()
: frame_(  NUtils::TConfig()["window"] )
, decoder_( new jpegdecoder )
, receiver_( decoder_.get() )
, rec_thread_( &receiver_ )
{}

baseviewer::~baseviewer()
{}

void baseviewer::start_stream()
{
    std::string ex = rec_thread_.exception();
    if( !ex.empty() )
    {
        throw std::logic_error( ex );
    }
    f_start_stream();
}

int baseviewer::stop_stream()
{
    return f_stop_stream();
}
