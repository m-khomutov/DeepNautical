/* 
 * File:   httpprotocol.h
 * Author: mkh
 *
 * Created on 2 марта 2023 г., 14:19
 */

#ifndef HTTPPROTOCOL_H
#define HTTPPROTOCOL_H

#include "baseprotocol.h"

class httpprotocol:public baseprotocol {
public:
    explicit httpprotocol( int b_sock );
    httpprotocol(const httpprotocol& orig) = delete;
    httpprotocol &operator =(const httpprotocol& orig) = delete;
    ~httpprotocol();

    void on_data( const uint8_t * data, int size ) override;
    void send_frame( const uint8_t * data, int size, float duration ) override;

private:
    void f_send_scene_list();
    void f_send_current_scene();

};

#endif /* HTTPPROTOCOL_H */

