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

THTTPapi::TRequest::TRequest( const std::string &data )
: origin( data )
{
    std::istringstream iss( origin );
    std::string s;
    while( std::getline( iss, s, '\n') ) // построчный разбор
    {
        if( method.empty() ) // метод не определен - это стартовая строка
        {
            std::istringstream is( s );
            std::vector<std::string> rc( std::istream_iterator<std::string>({ is }),
                                         std::istream_iterator<std::string>());
            if( rc.size() == 3 ) // формат стартовой строки: 'метод ури версия'
            {
                method = rc[0];
                uri = rc[1];
                version = rc[2];
            }
        }
        else // последующие строки - заголовочные формата: 'имя: значение'
        {
            size_t p = s.find( ":" );
            if( p != std::string::npos )
            {
                headers[s.substr(0, p)] = s.substr(p + 1, s.size() - (p + 2));
            }
        }
    }
}


THTTPapi::THTTPapi( int b_sock, int flags, TBasescreen *screen )
: TBaseprotocol( b_sock, flags )
, screen_( screen )
{}

THTTPapi::~THTTPapi()
{}

void THTTPapi::on_data( const uint8_t * data, int size )
{
    TRequest request( std::string((const char*)data, size) );
    std::cerr << request.origin << std::endl;

    if( !screen_ ) // нет объекта хранения сцен - нет возможности ответить
    {
        f_set_reply( (uint8_t const *)status_404, strlen( status_404 ) );
        f_reply();
    }
    // обработать контрольный запрос
    if( request.uri == "/scene?list" )
    {
        f_send_scene_list();
    }
    else if( request.uri == "/scene?get" )
    {
        f_send_current_scenes();
    }
    else if( request.uri.find( "/scene?set=" ) == 0 )
    {
        f_set_current_scene( request.uri.substr( 6 ) );
    }
    else // неизвестный запрос
    {
        f_set_reply( (uint8_t const *)status_404, strlen( status_404 ) );
        f_reply();
    }
}

void THTTPapi::do_write()
{
    // если что не доотправилось - доотправить
    if( sent_ < reply_.size() )
    {
        f_reply();
    }
}

void THTTPapi::send_frame( const uint8_t *, int )
{}

bool THTTPapi::can_send_frame() const
{
    return false;
}

void THTTPapi::write_error()
{
    f_set_reply( (uint8_t const *)status_404, strlen( status_404 ) );
    f_reply();
}

void THTTPapi::f_send_scene_list()
{
    std::string body = "{\"success\":true,\"scenes\":[";
    for( auto sc : screen_->get_scenes() )
    {
        body += "\"" + sc + "\",";
    }
    body.pop_back();
    body += "]}";

    std::string reply = std::string(status_200) + "Content-Length: " + std::to_string( body.size() ) + "\r\n\r\n" + body;

    f_set_reply( (uint8_t const *)reply.data(), reply.size() );
    f_reply();
}

void THTTPapi::f_send_current_scenes()
{
    std::vector< std::string > scenes = screen_->current_scenes();
    std::string body = "{\"success\":true,\"scenes\":[";
    for( const auto &sc : screen_->current_scenes() )
    {
        body += std::string("\"") + sc + "\",";
    }
    body.pop_back(); // убрать последнюю запятую
    body += "]}";
    std::string reply = std::string(status_200) + "Content-Length: " + std::to_string( body.size() ) + "\r\n\r\n" + body;

    f_set_reply( (uint8_t const *)reply.data(), reply.size() );
    f_reply();
}

void THTTPapi::f_set_current_scene( const std::string &query )
{
    // разобрать параметры (set и view)
    std::vector< THttpParameter > params = THttpParameter::parse( query );
    size_t view = 0;
    const std::string *scene = nullptr;

    for( const auto &p : params )
    {
        if( p.field == "set" )
        {
            scene = &p.value;  // имя сцены
        }
        else if( p.field == "view" && std::isdigit( p.value[0] ) )
        {
            view = p.value[0] - '0'; // номер точки обзора
        }
    }

    try
    {
        // поменять сцену
        if( scene )
        {
            screen_->set_scene( *scene, view );
        }
        // вернуть подтверждение
        std::string body = "{\"success\":true}";
        std::string reply = std::string(status_200) + "Content-Length: " + std::to_string( body.size() ) + "\r\n\r\n" + body;

        f_set_reply( (uint8_t const *)reply.data(), reply.size() );
        f_reply();
    }
    catch( const std::runtime_error &err )
    {
        std::cerr << "set scene " << scene << " error: " << err.what() << std::endl;

        f_set_reply( (uint8_t const *)status_404, strlen( status_404 ) );
        f_reply();
    }
}

void THTTPapi::f_set_reply( uint8_t const * data, size_t size )
{
    // собрать ответ
    reply_.resize( size );
    ::memcpy( reply_.data(), data, size );
    // пометить начало передачи
    sent_ = 0;
}

void THTTPapi::f_reply()
{
    int rc = ::send( fd_, reply_.data() + sent_, reply_.size() - sent_, flags_ );
    if( rc > 0 )
    {
        sent_ += rc;
    }
}
