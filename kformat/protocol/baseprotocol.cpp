/* 
 * File:   baseprotocol.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 14:59
 */

#include "baseprotocol.h"
#include "flvprotocol.h"
#include "httpapi.h"
#include <cstring>
#include <cerrno>
#include <iostream>

protocol_error::protocol_error( const std::string &what )
: std::runtime_error( what + std::string(" failed: ") + std::string(strerror( errno )) )
{}

baseprotocol *baseprotocol::create( basescreen *screen, const std::string &request, int sock, int flags )
{
    if( request.find( "\r\n\r\n" ) != std::string::npos )
    {
        if( request.find( "GET /stream?" ) != std::string::npos )
        {
            return new flvprotocol( sock, flags, request );
        }
        if( request.find( "GET /scene?" ) != std::string::npos )
        {
            return new httpapi( sock, flags, screen );
        }
    }
    return nullptr;
}

baseprotocol::baseprotocol( int sock, int flags )
: fd_( sock )
, flags_( flags )
{}

baseprotocol::~baseprotocol()
{}
