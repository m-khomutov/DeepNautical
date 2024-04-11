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

TProtocolError::TProtocolError( const std::string &what )
: std::runtime_error( what + std::string(" failed: ") + std::string(strerror( errno )) )
{}

TBaseprotocol *TBaseprotocol::create( basescreen *screen, const std::string &request, int sock, int flags )
{
    if( request.find( "\r\n\r\n" ) != std::string::npos )
    {
        auto params = http_param::parse( request );

        if( request.find( "GET /stream?" ) != std::string::npos )
        {
            const std::string *proto = nullptr;
            size_t view = 0;
            for( const auto &p : params )
            {
                if( p.field == "proto" )
                {
                    proto = &p.value;
                }
                else if( p.field == "view" && std::isdigit( p.value[0] ) )
                {
                    view = p.value[0] - '0';
                }
            }
            if( proto && *proto == "flv" )
            {
                return new flvprotocol( sock, flags, view );
            }
        }
        if( request.find( "GET /scene?" ) != std::string::npos )
        {
            return new httpapi( sock, flags, screen );
        }
    }
    return nullptr;
}

TBaseprotocol::TBaseprotocol( int sock, int flags )
: fd_( sock )
, flags_( flags )
{}

TBaseprotocol::~TBaseprotocol()
{}



http_param::http_param( const std::string &request, size_t begin, size_t end )
{
    size_t eq = request.find( '=', begin );
    if( eq != std::string::npos )
    {
        field.assign( request, begin, eq - begin );
        value.assign( request, eq + 1, (end - (eq + 1) ) );
    }
    else
    {
        field.assign( request, begin, end - begin );
    }
}

std::vector< http_param > http_param::parse( const std::string &request )
{
    std::vector< http_param > params;

    size_t begin = request.find( '?' );
    if( begin != std::string::npos )
    {
        ++begin;

        size_t end = std::string::npos;
        while( (end = request.find( '&', begin )) != std::string::npos )
        {
            params.emplace_back( request, begin, end );
            begin = end + 1;
        }

        params.emplace_back( request, begin, request.find( ' ', begin ) );
    }

    return params;
}
