/* 
 * File:   c_socket.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 10:34
 */

#ifndef FLVPROTOCOL_H
#define FLVPROTOCOL_H

#include "baseprotocol.h"

class flvprotocol: public baseprotocol {
public:
    explicit flvprotocol( int b_sock );
    flvprotocol(const flvprotocol& orig) = delete;
    flvprotocol & operator =(const flvprotocol& orig) = delete;
    ~flvprotocol();


    void on_data( const uint8_t * data, int size ) override;
    void send_frame( const uint8_t * data, int size, float duration ) override;
    
private:
    uint8_t tag_header_[12] { 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x11 };
    float timestamp_ { 0.0f };
};

#endif /* FLVPROTOCOL_H */
