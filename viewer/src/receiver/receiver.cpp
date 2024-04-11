/* 
 * File:   receiver.cpp
 * Author: mkh
 * 
 * Created on 26 января 2023 г., 9:32
 */

#include "receiver.h"
#include "../decoder/basedecoder.h"
#include <unistd.h>
#include <regex>

receiver_error::receiver_error( const std::string &what )
: std::runtime_error( what )
{
}

namespace
{
uint32_t from3bytes( uint8_t const *from )
{
    union
    {
        uint32_t value;
        uint8_t buf[sizeof(uint32_t)];
    } u;
    u.buf[0] = 0;
    memcpy( u.buf + 1, from, 3 );
    return be32toh( u.value );   
}
}  // namespace

flv_header::flv_header( uint8_t const* data )
{
    memcpy( signature_, data, 3 );
    version_ = data[3];
    audio_present_ = (data[4] >> 2) & 1;    
    video_present_ = (data[4]) & 1;    
    memcpy( &data_offset_, &data[5], sizeof(data_offset_) );
    data_offset_ = be32toh( data_offset_ );
}

bool flv_header::valid() const
{
    return signature_[0] == 'F' && 
           signature_[1] == 'L' &&
           signature_[2] == 'V' &&
           version_ == 1 &&
           video_present_ &&
           data_offset_ == 9;
}


flv_tag::flv_tag( uint8_t const *data )
{
    type_ = tag_type( data[0] );
    data_size_ = from3bytes( &data[1] );
    //timestamp_ = (data[7] << 24) | (from3bytes( &data[4] ));
    ::memcpy( &timestamp_, &data[4], sizeof(timestamp_) );
    timestamp_ = be64toh(timestamp_);
    //stream_id_ = from3bytes( &data[8] );
    frame_type_ = (data[12] >> 4) & 0x0f;
    codec_id_ = codec_id( (data[12]) & 0x0f );
}

bool flv_tag::valid() const
{
    return type_ == tag_type::video && data_size_ != 0 && codec_id_ == codec_id::jpeg;    
}


receiver::receiver( basedecoder *decoder )
: decoder_( decoder )
{
    try
    {
        std::string url = NUtils::TConfig()["url"];
        std::regex r("^([a-z]+)://([0-9\\.\\S]+):([0-9]{4,5})/([0-9])$"); // proto://host:port/context
        std::smatch cm;

        if( !(std::regex_match( url, cm, r )) || cm.size() != 5 )
        {
            throw receiver_error( std::string("invalid url ") + url );
        }
        server_host_ = cm[2].str();
        server_port_ = std::stoi( cm[3].str() );
        view_ = std::stoi( cm[4].str() );
    }
    catch (const std::regex_error &e)
    {
        throw receiver_error( e.what() );
    }
}

receiver::~receiver()
{
}

void receiver::run()
{
    f_start_connection();
    receiver::action = &receiver::f_receive_tag;
    
    std::vector< uint8_t > buffer( flv_tag::size );
    size_t expected = flv_tag::size, received = 0;
    while( running_.load() )
    {
        if( reconnect_delay_ < 100 )
            ++reconnect_delay_;

        ssize_t rc = connection_->receive( buffer.data() + received, expected );
        if( rc > 0 )
        {
            expected -= rc;
            received += rc;
            reconnect_delay_ = 0;
        }
        else if( reconnect_delay_ >= 100 )
        {
            f_start_connection();
            receiver::action = &receiver::f_receive_tag;
            expected = flv_tag::size;
        }
        if( expected == 0 )
        {
            try
            {
                expected = (this->*action)( buffer.data(), received );
            }
            catch( const std::runtime_error &e )
            {
                f_start_connection();
                receiver::action = &receiver::f_receive_tag;
                expected = flv_tag::size;
            }
            if( buffer.size() < expected )
            {
                buffer.resize( expected );    
            }
            received = 0;
        }
    }
}

void receiver::stop()
{
    running_.store( false );
}


void receiver::f_start_connection()
{
    while( running_.load() )
    {
        try
        {
            connection_.reset( new c_socket( server_host_, server_port_ ) );
            break;
        }
        catch( const std::runtime_error &e )
        {
            ::sleep( 1 );
        }
    }
    if( !connection_ )
    {
        throw receiver_error( "invalid connection" );
    }
    std::string request = "GET /stream?proto=flv&view=" + std::to_string(view_) + " HTTP/1.1\r\n"
                          "User-Agent: Viewer/0.0.1 (agat-aquarius)\r\n"
                          "Accept: */*\r\n"
                          "Accept-Encoding: identity\r\n"
                          "Host: " + server_host_ + ":" + std::to_string(server_port_) + "\r\n"
                          "Connection: Keep-Alive\r\n\r\n";
    connection_->send( (uint8_t const *)request.data(), request.size() );   

    std::string reply;
    int eol_count = 0;
    while( running_.load() )
    {
        char c;
        if( connection_->receive( (uint8_t *)&c, 1) == 1 )
        {
            if( !reply.empty() && reply.back() == '\r' && c == '\n' )
            {
                ++ eol_count;
            }
            else if( c != '\r' )
            {
                eol_count = 0;
            }
            reply += c;

            if( eol_count == 2 )
            {
                break;
            }
        }
    }
    uint8_t buffer[13];
    size_t rc = 0;
    while( running_.load() && rc < sizeof(buffer) )
    {
        size_t bs = connection_->receive( buffer, sizeof(buffer) );
        if( bs > 0 )
        {
            rc += bs;
        }
    }
    if( !flv_header( buffer ).valid() )
    {
        throw receiver_error( "invalid flv_header" );
    }
}

size_t receiver::f_receive_tag( uint8_t const *data, size_t size )
{
    if( size != flv_tag::size )
    {
        throw receiver_error( "invalid tag size" );
    }

    flv_tag tag( data );
    if( !tag.valid() )
    {
        throw receiver_error( "tag invalid" );
    }

    timestamp_ = tag.timestamp();
    receiver::action = &receiver::f_receive_body;
    
    return tag.data_size() - 1;
}

size_t receiver::f_receive_body( uint8_t const *data, size_t size )
{
    if( verify_ )
    {
        std::cerr << int64_t(NUtils::now() - timestamp_) << " nsec.\n";
    }

    decoder_->store( data, size, timestamp_ );
    receiver::action = &receiver::f_receive_size;
    return sizeof( uint32_t );
}

size_t receiver::f_receive_size( uint8_t const *, size_t size )
{
    if( size != sizeof(uint32_t) )
    {
        throw receiver_error( "invalid size" );
    }

    receiver::action = &receiver::f_receive_tag;
    return flv_tag::size;
}
