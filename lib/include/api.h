/*!
     \file httpapi.h
     \author mkh
     \date 2 марта 2023 г.
     \brief Заголовочный файл класса сетевого контрольного протокола формата HTTP.

     Данный файл содержит в себе состояние класса сетевого контрольного протокола формата HTTP, объявление его интерфейсов.
 */
#ifndef HTTPAPI_H
#define HTTPAPI_H

#include "protocol.h"

#include <map>
#include <string>
#include <vector>

namespace api
{

class protocol: public base::protocol
{
public:
    class request
    {
    public:
        explicit request( const std::string &s );
        request( const request& orig ) = delete;
        ~request() = default;

        request &operator =( const request& orig ) = delete;

        std::string origin;
        std::string method;
        std::string uri;
        std::string version;
        std::map< std::string, std::string > headers;
    };

    explicit protocol( int b_sock, int flags, base::screen *screen );
    protocol( const protocol& other ) = delete;
    ~protocol() = default;

    protocol &operator =( const protocol& other ) = delete;

    void on_data( const uint8_t * data, int size ) override;
    void do_write() override;
    void send_frame( const uint8_t * data, int size ) override;
    bool can_send_frame() const override;
    void write_error() override;

private:
    base::screen *screen_;
    size_t sent_ { 0 };
    std::vector< uint8_t > reply_;

private:
    void f_send_scene_list();
    void f_send_current_scenes();
    void f_set_current_scene( const std::string &query );
    void f_set_reply( uint8_t const * data, size_t size );
    void f_reply();
};

}  // namespace api
#endif /* HTTPAPI_H */

