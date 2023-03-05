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

httpprotocol::message::message( const std::string &data )
: origin( data )
{
    std::istringstream iss( origin );
    std::string s;
    while( std::getline( iss, s, '\n') )
    {
        if( method.empty() )
        {
            std::istringstream is( s );
            std::vector<std::string> rc( std::istream_iterator<std::string>({ is }),
                                         std::istream_iterator<std::string>());
            if( rc.size() == 3 )
            {
                method = rc[0];
                uri = rc[1];
                version = rc[2];
            }
        }
        else
        {
            size_t p = s.find( ":" );
            if( p != std::string::npos )
            {
                headers[s.substr(0, p)] = s.substr(p + 1, s.size() - (p + 2));
            }
        }
    }
}


httpprotocol::httpprotocol( int b_sock )
: baseprotocol( b_sock )
{
}

httpprotocol::~httpprotocol()
{
}

void httpprotocol::on_data( const uint8_t * data, int size )
{
    message request( std::string((const char*)data, size) );
    std::cerr << request.origin << std::endl;
    if( request.uri == "/scene?list" )
    {
        f_send_scene_list();
    }
    else if( request.uri == "/scene?get" )
    {
        f_send_current_scene();
    }
    else if( request.uri.find( "/scene?set=" ) == 0 )
    {
        f_set_current_scene( request.uri.substr( 11 ) );
    }
    else
    {
        ::write( fd_, status_404, strlen( status_404 ) );
    }
}

void httpprotocol::send_frame( const uint8_t *, int, float )
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

void httpprotocol::f_set_current_scene( const std::string &scene )
{
    try
    {
        baseservice::instance().set_scene( scene );
        f_send_current_scene();
    }
    catch( const std::runtime_error &err )
    {
        std::cerr << "set scene " << scene << " error: " << err.what() << std::endl;
        ::write( fd_, status_404, strlen( status_404 ) );
    }
}
