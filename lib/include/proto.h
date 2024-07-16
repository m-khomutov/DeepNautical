/*!
     \file baseprotocol.h
     \author mkh
     \date 25 января 2023 г.
     \brief Заголовочный файл базового класса сетевого протокола трансляции видеокадров.

     Данный файл содержит в себе состояние базового класса сетевого протокола трансляции видеокадров, объявление его интерфейсов.
 */

#ifndef BASEPROTOCOL_H
#define BASEPROTOCOL_H

#include <stdexcept>
#include <vector>

namespace base
{

class screen;

class protocolerror: public std::runtime_error
{
public:
    protocolerror( const std::string & what );
};

class protocol
{
public:
    static protocol * create( base::screen *screen, const std::string &request, int sock, int flags );

    explicit protocol( int sock, int flags );
    protocol( const protocol& other ) = delete;
    virtual ~protocol() = default;

    protocol &operator =( const protocol& other ) = delete;

    virtual void on_data( const uint8_t * data, int size ) = 0;
    virtual void do_write() = 0;
    virtual void send_frame( const uint8_t * data, int size ) = 0;
    virtual bool can_send_frame() const = 0;
    virtual void write_error() = 0;

    size_t view() const
    {
        return view_;
    }

protected:
    static const char * status_200;
    static const char * status_404;

    int fd_;
    int flags_;
    size_t view_ {0};
};

struct parameter
{
    std::string field;
    std::string value;

    parameter( const std::string &requst, size_t begin, size_t end );
    static std::vector< parameter > parse( const std::string &query );
};

}  // namespace base
#endif /* BASEPROTOCOL_H */

