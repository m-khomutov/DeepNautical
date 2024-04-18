/* 
 * File:   baseprotocol.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 14:59
 */

#include "baseprotocol.h"
#include "flvprotocol.h"
#include "mjpegprotocol.h"
#include "httpapi.h"
#include <cstring>
#include <cerrno>
#include <iostream>

TProtocolError::TProtocolError( const std::string &what )
: std::runtime_error( what + std::string(" failed: ") + std::string(strerror( errno )) )
{}

TBaseprotocol *TBaseprotocol::create( TBasescreen *screen, const std::string &request, int sock, int flags )
{
    if( request.find( "\r\n\r\n" ) != std::string::npos )
    {
        auto params = THttpParameter::parse( request );

        if( request.find( "GET /stream?" ) != std::string::npos ) // запрос на выдачу видеоконтента
        {
            // в параметрах должно быть имя протокола и номер точки обзора
            const std::string *proto = nullptr;
            size_t view = 0;
            for( const auto &p : params )
            {
                if( p.field == "proto" )
                {
                    proto = &p.value;  // имя протокола
                }
                else if( p.field == "view" && std::isdigit( p.value[0] ) )
                {
                    view = p.value[0] - '0'; // номер точки обзора
                }
            }
            if( proto )
            {
                if( *proto == "flv" )
                {
                    return new TFLVprotocol( sock, flags, view );
                }
                else if( *proto == "mjpeg" )
                {
                    return new TMjpegprotocol( sock, flags, view );
                }
            }
        }
        if( request.find( "GET /scene?" ) != std::string::npos ) // запрос на смену сцены
        {
            return new THTTPapi( sock, flags, screen );
        }
    }
    return nullptr; // неизвестный запрос
}


const char * TBaseprotocol::status_200 = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\n";
const char * TBaseprotocol::status_404 = "HTTP/1.1 404 Not Found\r\nAccess-Control-Allow-Origin: *\r\n\r\n";


TBaseprotocol::TBaseprotocol( int sock, int flags )
: fd_( sock )
, flags_( flags )
{}



THttpParameter::THttpParameter( const std::string &request, size_t begin, size_t end )
{
    size_t eq = request.find( '=', begin );
    if( eq != std::string::npos ) // вид параметра в строке: имя=значение
    {
        field.assign( request, begin, eq - begin );
        value.assign( request, eq + 1, (end - (eq + 1) ) );
    }
    else // вид параметра в строке: имя
    {
        field.assign( request, begin, end - begin );
    }
}

std::vector< THttpParameter > THttpParameter::parse( const std::string &request )
{
    std::vector< THttpParameter > params;

    size_t begin = request.find( '?' ); // начало списка параметров
    if( begin != std::string::npos )
    {
        ++begin;

        size_t end = std::string::npos;
        while( (end = request.find( '&', begin )) != std::string::npos ) // разделитель
        {
            params.emplace_back( request, begin, end );
            begin = end + 1;
        }

        params.emplace_back( request, begin, request.find( ' ', begin ) );
    }

    return params;
}
