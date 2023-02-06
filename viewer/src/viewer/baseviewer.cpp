/* 
 * File:   baseviewer.cpp
 * Author: mkh
 * 
 * Created on 6 февраля 2023 г., 16:09
 */

#include "baseviewer.h"
#ifdef QT_CORE_LIB
# include "qviewer.h"
#else
# include "gviewer.h"
#endif
#include "../decoder/jpegdecoder.h"


viewer_error::viewer_error( const std::string &what )
: std::runtime_error( what )
{
}

baseviewer *baseviewer::make( int argc, char *argv[] )
{
#ifdef QT_CORE_LIB
    return new qviewer;
#else
    return new g_viewer( argc, argv );
#endif
}

baseviewer::baseviewer()
: frame_(  utils::config()["window"] )
, decoder_( new jpegdecoder )
, receiver_( decoder_.get() )
, rec_thread_( &receiver_ )
{
}

baseviewer::~baseviewer()
{
}

void baseviewer::run()
{
    f_run();
}

int baseviewer::stop()
{
    return f_stop();
}
