/*!
     \file:   utils.h
     \author: mkh
     \date 26 января 2023 г.
     \brief Заголовочный файл вспомогательных классов и функций.

     Данный файл содержит в себе объявления вспомогательных классов и функций.
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

struct winsize
{
    int width = 800;
    int height = 600;

    winsize() = default;
    winsize( int w, int h)
        : width( w )
        , height( h )
    {}
    winsize( const char * from )
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
class threadsafe
{
public:
    class value {
    public:
        T& operator *()
        {
            return *value_;
        }
        T* operator ->()
        {
            return value_;
        }

        value( value && other )
        : mutex_( std::move( other.mutex_ ) )
        , value_( other.value_ )
        {}

    private:
        value( std::mutex *m, T *v )
        : mutex_( m, []( std::mutex *m_p ){ m_p->unlock(); } )
        , value_( v )
        {
            mutex_->lock();
        }

    private:
        std::shared_ptr< std::mutex > mutex_;
        T *value_;

        friend class threadsafe;
    };

public:
    threadsafe()
    : value_( new T )
    {}
    threadsafe( const threadsafe& other ) = delete;

    threadsafe &operator =( const threadsafe& other ) = delete;

    value guarded()
    {
        return value( &mutex_, value_.get() );
    }

private:
    std::mutex mutex_;
    std::unique_ptr< T > value_;
};


template< typename T >
class scoped_thread
{
 public:
     scoped_thread() = default;

     scoped_thread( T *obj )
         : object_( obj )
         , thread_( [this](){
             try {
                 object_->start();
             }
             catch ( const std::exception &e ) {
                 *exception_.guarded() = e.what();
             }
           })
     {}
     scoped_thread( scoped_thread const &other ) = delete;
     ~scoped_thread()
     {
         if( object_ )
         {
             object_->stop();
             if( thread_.joinable() )
             {
                 thread_.join();
             }
         }
     }

     scoped_thread &operator =( scoped_thread const &other ) = delete;

     std::string exception()
     {
         return *exception_.guarded();
     }

private:
    T *object_ { nullptr };
    std::thread thread_;

    threadsafe< std::string > exception_;
};

class settings
{
public:
    settings() = default;
    settings( int argc, char * argv[] );
    settings( settings const &other ) = delete;

    settings &operator =( settings const &other ) = delete;

    class variant
    {
    public:
        variant() = default;
        variant( int v );
        variant( std::string v );
        variant( winsize const &wsz );

        operator int() const;
        operator std::string() const;
        operator winsize() const;

    private:
        int ivalue_;
        std::string svalue_;
        winsize wvalue_;
    };
    using settings_t = std::map< std::string, variant >;
    
    variant &operator [](char const *key) const;
    
private:
    //! таблица конфигурации
    static settings_t settings_;

private:
    void f_from_file( char const *name );
};

struct frame
{
    enum colormode
    {
        Monochrome = 1,
        RGB = 3
    };

    std::vector< uint8_t > pixels;
    winsize wsize;
    int channels { colormode::RGB };
    uint64_t timestamp { std::numeric_limits< uint64_t >::max() };

    frame() = default;
    frame( winsize const &sz ) : wsize( sz ) {};
};


class jcodec {
public:
    class error: public std::runtime_error {
    public:
        error(const std::string & what);
    };

    jcodec();
    jcodec( const jcodec& other ) = delete;
    ~jcodec();

    jcodec &operator =( const jcodec& other ) = delete;

    bool decode( uint8_t const *data, size_t size, frame *dest );
    bool decode( char const *filename, frame *dest );

private:
    jpeg_decompress_struct cinfo_;
    jpeg_error_mgr jerr_;
};

bool exists( char const *filename );
void read_settings( char const *fname, std::function< void( const std::string& ) > foo );
bool read_header( const std::string &s, std::pair< std::string, std::string > *rc );

template< typename F >
void read_dir( const std::string &path, char const *filter, F &&func )
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
bool str2array( const std::string &s, T *rc )
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

inline uint32_t usec()
{
    timeval tv;
    gettimeofday( &tv, nullptr );

    return tv.tv_sec * 1000000 + tv.tv_usec;
}

}  // namespace utils

#endif /* UTILS_H */
