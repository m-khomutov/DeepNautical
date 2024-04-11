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

namespace NUtils
{
struct TGeometry
{
    int width = 800;
    int height = 600;

    TGeometry() = default;
    TGeometry( int w, int h)
    : width( w )
    , height( h )
    {}

    TGeometry( const char * from )
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
class TScopedThread
{
 public:
     TScopedThread() = default;
     explicit TScopedThread( T *obj )
     : object_( obj )
     , t_( [this](){ try { object_->run(); } catch ( const std::exception &e ) { std::cerr << e.what() <<std::endl; } } )
     {
     }
     TScopedThread( TScopedThread const &orig ) = delete;
     TScopedThread &operator =( TScopedThread const &rhs ) = delete;
     ~TScopedThread()
     {
         if( object_ )
         {
             object_->stop();
             if( t_.joinable() )
             {
                 t_.join();
             }
         }
     }

private:
     T *object_ { nullptr };
    std::thread t_;
};

class TConfig
{
public:
    TConfig() = default;
    TConfig( int argc, char * argv[] );
    TConfig( TConfig const &orig) = delete;
    TConfig &operator =( TConfig const &rhs ) = delete;

    class TVariant
    {
    public:
        TVariant() = default;
        TVariant( int v );
        TVariant( std::string v );
        TVariant( TGeometry const &v );

        operator int() const;
        operator std::string() const;
        operator TGeometry() const;

    private:
        int ivalue_;
        std::string svalue_;
        TGeometry gvalue_;
    };
    using fields_t = std::map< std::string, TVariant >;
    
    TVariant &operator [](char const *key) const;
    
private:
    static fields_t fields_;

private:
    void f_read_file( char const *name );
};

struct TImage
{
    std::vector< uint8_t > pixels;
    TGeometry window;
    int channels { 3 };
    uint64_t timestamp { 0xff };

    TImage() = default;
    TImage( TGeometry const &win ) : window( win ) {};
};

class TJpegCodec {
public:
    class TError: public std::runtime_error {
    public:
        TError(const std::string & what);
    };
    
    TJpegCodec();
    TJpegCodec(const TJpegCodec& orig) = delete;
    TJpegCodec &operator =(const TJpegCodec& orig) = delete;
    ~TJpegCodec();
    
    bool decode( uint8_t const *data, size_t size, TImage *img );
    bool decode( char const *filename, TImage *img );

private:
    jpeg_decompress_struct cinfo_;
    jpeg_error_mgr jerr_;
};

template< typename T >
class TSafeguard {
public:
    class TGuard {
    public:
        T& operator *()
        {
            return *value_;
        }
        T* operator ->()
        {
            return value_;
        }

        TGuard( TGuard && rhs )
        : m_( std::move( rhs.m_ ) )
        , value_( rhs.value_ )
        {}

    private:
        TGuard( std::mutex *m, T *v )
        : m_( m, []( std::mutex *m_p ){ m_p->unlock(); } )
        , value_( v )
        {
            m_->lock();
        }

    private:
        std::shared_ptr< std::mutex > m_;
        T *value_;
        friend class TSafeguard;
    };

public:
    TSafeguard()
    : value_( new T )
    {}
    TSafeguard( const TSafeguard& orig ) = delete;
    TSafeguard &operator =( const TSafeguard& orig ) = delete;

    TGuard get()
    {
        return TGuard( &mutex_, value_.get() );
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

}  // namespace NUtils

#endif /* UTILS_H */
