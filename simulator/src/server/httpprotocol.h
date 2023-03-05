/* 
 * File:   httpprotocol.h
 * Author: mkh
 *
 * Created on 2 марта 2023 г., 14:19
 */

#ifndef HTTPPROTOCOL_H
#define HTTPPROTOCOL_H

#include "baseprotocol.h"
#include <map>
#include <string>

class httpprotocol:public baseprotocol {
public:
    class message {
    public:
        explicit message( const std::string &s );
        message(const message& orig) = delete;
        message &operator =(const message& orig) = delete;
        ~message() = default;

	std::string origin;
        std::string method;
        std::string uri;
        std::string version;
        std::map< std::string, std::string > headers;
    };

    explicit httpprotocol( int b_sock );
    httpprotocol(const httpprotocol& orig) = delete;
    httpprotocol &operator =(const httpprotocol& orig) = delete;
    ~httpprotocol();

    void on_data( const uint8_t * data, int size ) override;
    void send_frame( const uint8_t * data, int size, float duration ) override;

private:
    void f_send_scene_list();
    void f_send_current_scene();
    void f_set_current_scene( const std::string &scene );

};

#endif /* HTTPPROTOCOL_H */

