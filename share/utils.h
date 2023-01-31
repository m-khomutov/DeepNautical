/* 
 * File:   utils.h
 * Author: mkh
 *
 * Created on 26 января 2023 г., 9:03
 */

#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <string>
#include <map>
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
     scoped_thread( scoped_thread const &orig ) = delete;
     scoped_thread &operator =( scoped_thread const &rhs ) = delete;
     ~scoped_thread()
     {
         object_->stop();
         t_.join();
     }

private:
    T *object_;
    std::thread t_;
};

class config
{
public:
    config() = default;
    config( int argc, char * argv[] );
    config( config const &orig) = delete;
    config &operator =( config const &rhs ) = delete;

    class variant
    {
    public:
	variant() = default;
        variant( int v );
        variant( char const *v );
        variant( geometry const &v );

        operator int() const;
        operator std::string() const;
        operator geometry() const;

    private:
        int ivalue_;
        std::string svalue_;
	geometry gvalue_;
    };
    using fields_t = std::map< std::string, variant >;
    
    variant &operator [](char const *key) const;
    
private:
    inline static fields_t fields_ = fields_t();
};

}  // namespace utils

#endif /* UTILS_H */

