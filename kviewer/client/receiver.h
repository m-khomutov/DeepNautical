/* 
 * File:   receiver.h
 * Author: mkh
 *
 * Created on 26 января 2023 г., 9:32
 */

#ifndef RECEIVER_H
#define RECEIVER_H

#include "c_socket.h"
#include "utils.h"
#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

class receiver_error: public std::runtime_error
{
public:
    receiver_error( const std::string & what );
};

class flv_header
{
public:
    flv_header() = default;
    explicit flv_header( uint8_t const *data );

    bool valid() const;

private:
    char signature_[3];
    uint8_t version_ { 1 };
    bool audio_present_ { false };
    bool video_present_ { true };
    uint32_t data_offset_ = 0;
};

class flv_tag
{
public:
    enum { size = 13 };

    flv_tag() = default;
    explicit flv_tag( uint8_t const *data );

    bool valid() const;
    uint32_t data_size() const
    {
        return data_size_;
    }
    uint64_t timestamp() const
    {
        return timestamp_;
    }

private:
    enum tag_type { audio = 8, video = 9 };
    enum codec_id { jpeg = 1 };
    
    tag_type type_ { tag_type::video };
    uint32_t data_size_ { 0 };
    uint64_t timestamp_ { 0ul };
    uint32_t stream_id_ { 0 };
    uint8_t frame_type_ { 1 };
    uint8_t codec_id_ { codec_id::jpeg };
};

class basedecoder;

class receiver {
public:
    explicit receiver( basedecoder *decoder );
    receiver(const receiver& orig) = delete;
    receiver &operator =(const receiver& orig) = delete;
    ~receiver();

    /*!
       \brief запускает получение видеопотока с сетевого соединения
     */
    void start_listening_network();
    /*!
       \brief Останавливает получение видеопотока с сетевого соединения
     */
    void stop_listening_network();


private:
    size_t view_ {0};
    basedecoder *decoder_;
    std::atomic< bool > running_ { true };
    std::unique_ptr< TCsocket > connection_;
    std::string server_host_;
    uint16_t server_port_;
    size_t (receiver::*action)(uint8_t const *, size_t);
    uint64_t timestamp_ { 0ul };
    bool verify_ { bool(NUtils::TConfig()["verify"]) };
    int reconnect_delay_ { 0 };

private:
    void f_start_connection();
    size_t f_receive_tag( uint8_t const *data, size_t size );
    size_t f_receive_body( uint8_t const *data, size_t size );
    size_t f_receive_size( uint8_t const *data, size_t size );
};

#endif /* RECEIVER_H */
