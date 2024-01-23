/* 
 * File:   httpapi.h
 * Author: mkh
 *
 * Created on 2 марта 2023 г., 14:19
 */

#ifndef HTTPAPI_H
#define HTTPAPI_H

#include <kformat.h>
#include <map>
#include <string>
#include <vector>

class httpapi:public baseprotocol {
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

    explicit httpapi( int b_sock, basescreen *screen );
    httpapi(const httpapi& orig) = delete;
    httpapi &operator =(const httpapi& orig) = delete;
    ~httpapi();

    void on_data( const uint8_t * data, int size ) override;
    void do_write() override;
    void send_frame( const uint8_t * data, int size, float duration ) override;

private:
    basescreen *screen_;
    size_t sent_ { 0 };
    std::vector< uint8_t > reply_;

private:
    void f_send_scene_list();
    void f_send_current_scene();
    void f_set_current_scene( const std::string &scene );
    void f_set_reply( uint8_t const * data, size_t size );
    void f_reply();

};

#endif /* HTTPAPI_H */

