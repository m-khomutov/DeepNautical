/* 
 * File:   c_socket.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 10:34
 */

#ifndef FLVPROTOCOL_H
#define FLVPROTOCOL_H

#include "baseprotocol.h"
#include <vector>

class flvprotocol: public baseprotocol {
public:
    explicit flvprotocol( int b_sock, int flags, const std::string& request );
    flvprotocol(const flvprotocol& orig) = delete;
    flvprotocol & operator =(const flvprotocol& orig) = delete;
    ~flvprotocol();


    void on_data( const uint8_t * data, int size ) override;
    void do_write() override;
    void send_frame( const uint8_t * data, int size, float duration ) override;
    bool can_send_frame() const override;
    void write_error() override;
    
    size_t view() const override
    {
        return view_;
    }

private:
    std::vector< uint8_t > http_flv_header_;
    std::vector< uint8_t > flv_frame_;
    size_t sent_from_header_ { 0 };
    size_t sent_from_frame_ { 0 };
    /* TagType   | UI8 = 9
     * DataSize  | UI24
     * Timestamp | UI64
     * FrameType | UB[4] = 1
     * CodecID   | UB[4] = 1
     */
    uint8_t tag_header_[13] { 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x11 };
    uint64_t timestamp_ { 0ul };
    size_t view_ {0};

private:
    void f_send_header();
    void f_send_frame();
};

#endif /* FLVPROTOCOL_H */
