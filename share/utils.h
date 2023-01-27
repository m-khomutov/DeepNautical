/* 
 * File:   utils.h
 * Author: mkh
 *
 * Created on 26 января 2023 г., 9:03
 */

#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <thread>

namespace utils
{
struct geometry
{
    int width = 800;
    int height = 600;
    geometry() = default;
    geometry( int w, int h)
    : width( w )
    , height( h )
    {
    }
    geometry( const char * from )
    {
        char *ptr;
        int w = strtol( from, &ptr, 10 );
        if( w && ptr )
        {
            int h = strtol( ptr + 1, nullptr, 10 );
            if( h )
            {
                width = w;
                height = h;
            }
        }
    }
};

template< typename T >
class scoped_thread
{
 public:
     explicit scoped_thread( T *obj )
     : object_(obj)
     , t_([this](){ object_->run(); } )
     {
     }
     ~scoped_thread()
     {
         object_->stop();
         t_.join();
     }

private:
    T *object_;
    std::thread t_;
};

}  // namespace utils

#endif /* UTILS_H */

