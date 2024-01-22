/* 
 * File:   baseprotocol.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 14:59
 */

#include "baseprotocol.h"
#include "flvprotocol.h"
#include <cstring>
#include <cerrno>
#include <iostream>

protocol_error::protocol_error( const std::string &what )
: std::runtime_error( what + std::string(" failed: ") + std::string(strerror( errno )) )
{}

baseprotocol *baseprotocol::create( const std::string &request, int sock )
{
    if( request.find( "\r\n\r\n" ) != std::string::npos )
    {
        if( request.find( "GET /stream?proto=flv HTTP/1.1\r\n" ) != std::string::npos )
        {
            return new flvprotocol( sock );
        }
    }
    return nullptr;
}

baseprotocol::baseprotocol( int sock )
: fd_( sock )
{}

baseprotocol::~baseprotocol()
{}
