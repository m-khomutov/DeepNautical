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
#include <cstring>
#include <dirent.h>
#include <sys/time.h>
extern "C"
{
 #include <jpeglib.h>
 #include <jerror.h>
}
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <iterator>
#include <sstream>
#include <map>
#include <memory>
#include <mutex>
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
        variant( std::string v );
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
    static fields_t fields_;

private:
    void f_read_file( char const *name );
};

struct image
{
    std::vector< uint8_t > pixels;
    utils::geometry window;
    int channels { 3 };
    uint64_t timestamp { 0xff };

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
    bool decode( char const *filename, image *img );

private:
    jpeg_decompress_struct cinfo_;
    jpeg_error_mgr jerr_;
};

template< typename T >
class safeguard {
public:
    class guard {
    public:
        T& operator *()
        {
            return *value_;
        }
        T* operator ->()
        {
            return value_;
        }

        guard( guard && rhs )
        : m_( std::move( rhs.m_ ) )
        , value_( rhs.value_ )
        {}

    private:
        guard( std::mutex *m, T *v )
        : m_( m, []( std::mutex *m_p ){ m_p->unlock(); } )
        , value_( v )
        {
            m_->lock();
        }

    private:
        std::shared_ptr< std::mutex > m_;
        T *value_;
        friend class safeguard;
    };

public:
    safeguard()
    : value_( new T )
    {}
    safeguard(const safeguard& orig) = delete;
    safeguard &operator =(const safeguard& orig) = delete;

    guard get()
    {
        return guard( &mutex_, value_.get() );
    }

private:
    std::mutex mutex_;
    std::unique_ptr< T > value_;
};

bool file_exists( char const *filename );
void read_config( char const *fname,
                  std::function< void( const std::string& ) > foo );
bool str2key( const std::string &s, std::pair< std::string, std::string > *rc );

template< typename F >
void read_directory( const std::string &path, char const *filter, F &&func )
{
    DIR *dir;
    struct dirent *entry;
    if( !(dir = opendir( path.c_str() ) ) )
        throw std::runtime_error( path + std::string(" error: ") + std::string(strerror( errno ) ) );

    while( (entry = readdir(dir))  )
    {
        if( entry->d_name[0] != '.' && entry->d_type == DT_REG && strstr( entry->d_name, filter ) )
        {
            func( entry->d_name );
        }
    }
    closedir( dir );
}

template< typename T >
bool str2vec( const std::string &s, T *rc )
{
    std::istringstream iss(s);
    std::vector< std::string > v( (std::istream_iterator<std::string>(iss)),
                                   std::istream_iterator<std::string>() );
    if( v.size() == sizeof(T) / sizeof(float) )
    {
        for( size_t i(0); i < v.size(); ++i )
        {
            try
            {
                (*rc)[i] = std::stof(v[i]);
            }
            catch( const std::logic_error & err )
            {
                std::cerr << __PRETTY_FUNCTION__ << " error: " << err.what() <<std::endl;
                return false;
            }
        }
        return true;
    }
    return false;
}
 
inline uint64_t now()
{
    timespec ts;
    clock_gettime( CLOCK_REALTIME, &ts );

    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

}  // namespace utils

#endif /* UTILS_H */
