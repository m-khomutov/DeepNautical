/* 
 * File:   httpapi.cpp
 * Author: mkh
 * 
 * Created on 2 марта 2023 г., 14:19
 */

#include "httpapi.h"
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <cstring>
#include <iostream>
#include <iterator>
#include <sstream>

namespace
{
const char status_200[] = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\n";
const char status_404[] = "HTTP/1.1 404 Not Found\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
} // namespace

httpapi::message::message( const std::string &data )
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


httpapi::httpapi( int b_sock, int flags, basescreen *screen )
: baseprotocol( b_sock, flags )
, screen_( screen )
{
}

httpapi::~httpapi()
{
}

void httpapi::on_data( const uint8_t * data, int size )
{
    message request( std::string((const char*)data, size) );
    std::cerr << request.origin << std::endl;

    if( !screen_ )
    {
        f_set_reply( (uint8_t const *)status_404, strlen( status_404 ) );
        f_reply();
    }

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
        f_set_reply( (uint8_t const *)status_404, strlen( status_404 ) );
        f_reply();
    }
}

void httpapi::do_write()
{
    if( sent_ < reply_.size() )
    {
        f_reply();
    }
}

void httpapi::send_frame( const uint8_t *, int, float )
{}

bool httpapi::can_send_frame() const
{
    return false;
}

void httpapi::f_send_scene_list()
{
    std::string body = "{\"success\":true,\"scenes\":[";
    for( auto sc : screen_->scenes() )
    {
        body += "\"" + sc + "\",";
    }
    body.pop_back();
    body += "]}";
    std::string reply = std::string(status_200) + "Content-Length: " + std::to_string( body.size() ) + "\r\n\r\n" + body;
    f_set_reply( (uint8_t const *)reply.data(), reply.size() );
    f_reply();
}

void httpapi::f_send_current_scene()
{
    std::string body = "{\"success\":true,\"scene\":\"" + screen_->current_scene() + "\"}";
    std::string reply = std::string(status_200) + "Content-Length: " + std::to_string( body.size() ) + "\r\n\r\n" + body;
    f_set_reply( (uint8_t const *)reply.data(), reply.size() );
    f_reply();
}

void httpapi::f_set_current_scene( const std::string &scene )
{
    try
    {
        screen_->set_scene( scene );
        f_send_current_scene();
    }
    catch( const std::runtime_error &err )
    {
        std::cerr << "set scene " << scene << " error: " << err.what() << std::endl;
        f_set_reply( (uint8_t const *)status_404, strlen( status_404 ) );
        f_reply();
    }
}

void httpapi::f_set_reply( uint8_t const * data, size_t size )
{
    reply_.resize( size );
    ::memcpy( reply_.data(), data, size );
    sent_ = 0;
}

void httpapi::f_reply()
{
    int rc = ::send( fd_, reply_.data() + sent_, reply_.size() - sent_, flags_ );
    if( rc > 0 )
    {
        sent_ += rc;
    }
}
