/*!
     \file flvprotocol.h
     \author mkh
     \date 25 января 2023 г.
     \brief Заголовочный файл класса сетевого протокола формата FLV (Flash Video) трансляции видеокадров.

     Данный файл содержит в себе состояние класса сетевого протокола формата FLV трансляции видеокадров, объявление его интерфейсов.
 */

#ifndef FLVPROTOCOL_H
#define FLVPROTOCOL_H

#include "protocol.h"
#include <vector>

namespace flv
{

class protocol: public base::protocol
{
public:
    explicit protocol( int b_sock, int flags, size_t view );
    protocol( const protocol& other ) = delete;
    ~protocol() = default;

    protocol & operator =( const protocol& other ) = delete;

    void on_data( const uint8_t * data, int size ) override;
    void do_write() override;
    void send_frame( const uint8_t * data, int size ) override;
    bool can_send_frame() const override;
    void write_error() override;

private:
    std::vector< uint8_t > http_flv_header_;
    std::vector< uint8_t > flv_frame_;
    size_t sent_from_header_ { 0 };
    size_t sent_from_frame_ { 0 };
    /*!
        \brief заголовок тега формата FLV:
        Поле              | Размер
        ----------------- | ------------
        TagType           | UI8 = 9
        DataSize          | UI24
        Timestamp         | UI24
        TimestampExtended | UI8
        FrameType         | UB[4] = 1
        CodecID           | UB[4] = 1
     */
    uint8_t tag_header_[9] { 9, 0, 0, 0, 0, 0, 0, 0, 0x11 };
    uint32_t timestamp_ { 0ul };

private:
    void f_send_header();
    void f_send_frame();
};

}  // namespace flv
#endif /* FLVPROTOCOL_H */
