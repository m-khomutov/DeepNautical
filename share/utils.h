/* 
 * File:   utils.h
 * Author: mkh
 *
 * Created on 26 января 2023 г., 9:03
 */

#ifndef UTILS_H
#define UTILS_H

#include <cstddef>
#include <cstdio>
#include <cstdint>
extern "C"
{
 #include <jpeglib.h>
 #include <jerror.h>
}
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <vector>

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
     : object_( obj )
     , t_( [this](){ try { object_->run(); } catch ( const std::exception &e ) { std::cerr << e.what() <<std::endl; } } )
     {
     }
     scoped_thread( scoped_thread const &orig ) = delete;
     scoped_thread &operator =( scoped_thread const &rhs ) = delete;
     ~scoped_thread()
     {
         object_->stop();
         if( t_.joinable() )
         {
             t_.join();
         }
     }

private:
    T *object_;
    std::thread t_;
};

enum class graphicsdim { dim2D, dim3D };

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
        variant( graphicsdim v );

        operator int() const;
        operator std::string() const;
        operator geometry() const;
        operator graphicsdim() const;

    private:
        int ivalue_;
        std::string svalue_;
        geometry gvalue_;
        graphicsdim dimvalue_;
    };
    using fields_t = std::map< std::string, variant >;
    
    variant &operator [](char const *key) const;
    
private:
    static fields_t fields_;
};

struct image
{
    std::vector< uint8_t > pixels;
    utils::geometry window;
    int channels { 3 };
    uint32_t timestamp { 0xff };

    image() = default;
    image( utils::geometry const &win ) : window( win ) {};
};

class jpeg_codec {
public:
    class error: public std::runtime_error {
    public:
        error(const std::string & what);
    };
    
    jpeg_codec();
    jpeg_codec(const jpeg_codec& orig) = delete;
    jpeg_codec &operator =(const jpeg_codec& orig) = delete;
    ~jpeg_codec();
    
    bool decode( uint8_t const *data, size_t size, image *img );

private:
    jpeg_decompress_struct cinfo_;
    jpeg_error_mgr jerr_;
};

bool file_exists( char const *filename );
}  // namespace utils

#endif /* UTILS_H */
