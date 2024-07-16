/*!
     \file baseframe.h
     \author mkh
     \date 25 января 2023 г.
     \brief Заголовочный файл базового класса представления видеокадра.

     Данный файл содержит в себе состояние базового класса представления видеокадра, объявление его интерфейсов.
 */

#ifndef BASEFRAME_H
#define BASEFRAME_H

#include "utils.h"
#include <chrono>

namespace base
{

class protocol;

class frame
{
public:
    using time_point_t = std::chrono::time_point< std::chrono::high_resolution_clock >;

    frame( const utils::winsize &sz, int duration );
    frame( const frame& other ) = delete;
    virtual ~frame();

    frame &operator =( const frame& other ) = delete;

    virtual uint8_t *buffer( size_t view, int width, int height ) = 0;
    virtual void prepare_buffer( size_t view ) = 0;

    bool send_buffer( protocol * );

    int width() const
    {
        return size_.width;
    }
    int height() const
    {
        return size_.height;
    }
    float expired( time_point_t *ts ) const;
    
protected:
    using pixels = std::vector< uint8_t >;

    utils::winsize size_;
    std::chrono::milliseconds duration_;
    std::vector< pixels > rgb_;

private:
    virtual bool f_send_buffer( protocol * p ) = 0;

};

}  // namespace base
#endif /* BASEFRAME_H */

