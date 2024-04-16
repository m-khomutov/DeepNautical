/* 
 * File:   receiver.cpp
 * Author: mkh
 * 
 * Created on 26 января 2023 г., 9:32
 */

#include "receiver.h"
#include "basedecoder.h"

#include <unistd.h>
#include <regex>

TReceiverError::TReceiverError( const std::string &what )
: std::runtime_error( what )
{}

namespace
{
// 3-х байтовую перменную из big-endian
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

TFLVheader::TFLVheader( uint8_t const* data )
{
    memcpy( signature_, data, 3 );
    version_ = data[3];
    audio_present_ = (data[4] >> 2) & 1;    
    video_present_ = (data[4]) & 1;    
    memcpy( &data_offset_, &data[5], sizeof(data_offset_) );
    data_offset_ = be32toh( data_offset_ );
}

bool TFLVheader::valid() const
{
    return signature_[0] == 'F' && 
           signature_[1] == 'L' &&
           signature_[2] == 'V' &&
           version_ == 1 &&
           video_present_ &&
           data_offset_ == 9;
}


TFLVtag::TFLVtag( uint8_t const *data )
{
    type_ = ETagType( data[0] );
    data_size_ = from3bytes( &data[1] );
    /* Реально в спецификации так
       timestamp_ = (data[7] << 24) | (from3bytes( &data[4] ));
       stream_id_ = from3bytes( &data[8] );
     */
    ::memcpy( &timestamp_, &data[4], sizeof(timestamp_) );
    timestamp_ = be64toh(timestamp_);
    frame_type_ = (data[12] >> 4) & 0x0f;
    codec_id_ = ECodecID( (data[12]) & 0x0f );
}

bool TFLVtag::valid() const
{
    return type_ == ETagType::video && data_size_ != 0 && codec_id_ == ECodecID::jpeg;
}


TReceiver::TReceiver( TBasedecoder *decoder )
: decoder_( decoder )
{
    // серверный урл из строки конфигурации
    try
    {
        std::string url = NUtils::TConfig()["url"];
        std::regex r("^([a-z]+)://([0-9\\.\\S]+):([0-9]{4,5})/([0-9])$"); // proto://host:port/context
        std::smatch cm;

        if( !(std::regex_match( url, cm, r )) || cm.size() != 5 )
        {
            throw TReceiverError( std::string("invalid url ") + url );
        }
        server_host_ = cm[2].str();
        server_port_ = std::stoi( cm[3].str() );
        view_ = std::stoi( cm[4].str() );
    }
    catch (const std::regex_error &e)
    {
        throw TReceiverError( e.what() );
    }
}

TReceiver::~TReceiver()
{}

void TReceiver::start_listening_network()
{
    f_start_connection(); // отправляется запрос на стримминг
    TReceiver::action = &TReceiver::f_receive_tag; // начальное действие автомата - принять тэг
    
    std::vector< uint8_t > buffer( TFLVtag::size );
    size_t expected = TFLVtag::size, received = 0; // размеры ожидаемый, принятый
    while( running_.load() )
    {
        if( reconnect_delay_ < 100 )
            ++reconnect_delay_;

        ssize_t rc = connection_->receive( buffer.data() + received, expected );
        if( rc > 0 ) // приняли данных из сети
        {
            expected -= rc; // ждем поменьше
            received += rc; // приняли побольше
            reconnect_delay_ = 0; // ожидание ожидаемо обнуляется
        }
        else if( reconnect_delay_ >= 100 ) // долго ждем, похоже на проблемы. Надо реконнектиться
        {
            f_start_connection();
            TReceiver::action = &TReceiver::f_receive_tag;
            expected = TFLVtag::size;
        }
        if( expected == 0 ) // все пришло. Обрабатываем текущей командой автомата.
        {
            try
            {
                expected = (this->*action)( buffer.data(), received );
            }
            catch( const std::runtime_error &e )
            {
                // если что-то пошло не так, реконнектимся
                f_start_connection();
                TReceiver::action = &TReceiver::f_receive_tag;
                expected = TFLVtag::size;
            }
            if( buffer.size() < expected )
            {
                buffer.resize( expected );    
            }
            received = 0;
        }
    }
}

void TReceiver::stop_listening_network()
{
    running_.store( false );
}

void TReceiver::register_verify_callback( verify_callback_t cb )
{
    verify_callback_ = cb;
}

void TReceiver::f_start_connection()
{
    while( running_.load() )
    {
        try
        {
            // физическое соединение
            connection_.reset( new TCsocket( server_host_, server_port_ ) );
            break;
        }
        catch( const std::runtime_error &e )
        {
            ::sleep( 1 );
        }
    }
    if( !connection_ )
    {
        throw TReceiverError( "invalid connection" );
    }

    // запрос на выдачу контента
    std::string request = "GET /stream?proto=flv&view=" + std::to_string(view_) + " HTTP/1.1\r\n"
                          "User-Agent: Viewer/0.0.1 (agat-aquarius)\r\n"
                          "Accept: */*\r\n"
                          "Accept-Encoding: identity\r\n"
                          "Host: " + server_host_ + ":" + std::to_string(server_port_) + "\r\n"
                          "Connection: Keep-Alive\r\n\r\n";
    connection_->send( (uint8_t const *)request.data(), request.size() );   

    // прием контента
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
    if( !TFLVheader( buffer ).valid() )
    {
        throw TReceiverError( "invalid flv_header" );
    }
}

// команда автомата принять FLV тег
size_t TReceiver::f_receive_tag( uint8_t const *data, size_t size )
{
    if( size != TFLVtag::size )
    {
        throw TReceiverError( "invalid tag size" );
    }

    TFLVtag tag( data );
    if( !tag.valid() )
    {
        throw TReceiverError( "tag invalid" );
    }

    timestamp_ = tag.timestamp();
    TReceiver::action = &TReceiver::f_receive_body;
    
    return tag.data_size() - 1;
}

// команда автомата принять тело FLV кадра
size_t TReceiver::f_receive_body( uint8_t const *data, size_t size )
{
    if( verify_callback_ ) // вызвать колбэк вывода временной задержки, если он зарегистрирован
    {
        verify_callback_( timestamp_ );
    }

    decoder_->save_frame( data, size, timestamp_ );
    TReceiver::action = &TReceiver::f_receive_size; // изменить действие автомата
    return sizeof( uint32_t );
}

// команда автомата принять тело FLV кадра
size_t TReceiver::f_receive_size( uint8_t const *, size_t size )
{
    if( size != sizeof(uint32_t) )
    {
        throw TReceiverError( "invalid size" );
    }

    TReceiver::action = &TReceiver::f_receive_tag; // изменить действие автомата
    return TFLVtag::size;
}
