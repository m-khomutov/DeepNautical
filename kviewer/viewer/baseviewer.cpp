/* 
 * File:   baseviewer.cpp
 * Author: mkh
 * 
 * Created on 6 февраля 2023 г., 16:09
 */

#include "baseviewer.h"
#include "jpegdecoder.h"


TBaseviewerError::TBaseviewerError( const std::string &what )
: std::runtime_error( what )
{
}

TBaseviewer::TBaseviewer()
: frame_(  NUtils::TConfig()["window"] )
, decoder_( new jpegdecoder )
, receiver_( decoder_.get() )
, rec_thread_( &receiver_ )
{}

void TBaseviewer::start_stream()
{
    // проверить исключение. Могло быть сгенерировано в потоке ресивера
    std::string ex = rec_thread_.exception();
    if( !ex.empty() )
    {
        throw std::logic_error( ex );
    }
    // все нормально. Можно начать прием
    f_start_stream();
}

int TBaseviewer::stop_stream()
{
    return f_stop_stream();
}

void TBaseviewer::register_verify_callback( TReceiver::verify_callback_t cb )
{
    receiver_.register_verify_callback( cb );
}
