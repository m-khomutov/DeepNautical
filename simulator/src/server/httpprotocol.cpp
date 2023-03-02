/* 
 * File:   httpprotocol.cpp
 * Author: mkh
 * 
 * Created on 2 марта 2023 г., 14:19
 */

#include "httpprotocol.h"
#include "../service/baseservice.h"
#include <unistd.h>
#include <cstring>

namespace
{
const char status_200[] = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\n";
const char status_404[] = "HTTP/1.1 404 Not Found\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
} // namespace

httpprotocol::httpprotocol( int b_sock )
: baseprotocol( b_sock )
{
}

httpprotocol::~httpprotocol()
{
}

void httpprotocol::on_data( const uint8_t * data, int size )
{
    std::string request( (const char*)data, size );
    std::cerr << request << std::endl;
    if( request.find( "GET /scene?list HTTP/1.1\r\n" ) != std::string::npos )
    {
        f_send_scene_list();
    }
    else if( request.find( "GET /scene?current HTTP/1.1\r\n" ) != std::string::npos )
    {
        f_send_current_scene();
    }
    else
    {
        ::write( fd_, status_404, strlen( status_404 ) );
    }
}

void httpprotocol::send_frame( const uint8_t * data, int size, float duration )
{
}

void httpprotocol::f_send_scene_list()
{
    std::string body = "{\"success\":true,\"scenes\":[";
    for( auto sc : baseservice::instance().scenes() )
    {
        body += "\"" + sc + "\",";
    }
    body.pop_back();
    body += "]}";
    std::string reply = std::string(status_200) + "Content-Length: " + std::to_string( body.size() ) + "\r\n\r\n";
    ::write( fd_, reply.data(), reply.size() );
    ::write( fd_, body.data(), body.size() );   
}

void httpprotocol::f_send_current_scene()
{
    std::string body = "{\"success\":true,\"scene\":\"" + baseservice::instance().current_scene() + "\"}";
    std::string reply = std::string(status_200) + "Content-Length: " + std::to_string( body.size() ) + "\r\n\r\n";
    ::write( fd_, reply.data(), reply.size() );
    ::write( fd_, body.data(), body.size() );   
}
