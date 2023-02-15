/* 
 * File:   utils.cpp
 * Author: mkh
 * 
 * Created on 31 января 2023 г., 10:37
 */

#include "utils.h"
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

utils::config::fields_t utils::config::fields_ = utils::config::fields_t();

utils::config::variant::variant( int v )
: ivalue_( v )
{
}

utils::config::variant::variant( char const *v )
: svalue_( v )
{
}


utils::config::variant::variant( utils::geometry const &v )
: gvalue_( v )
{
}

utils::config::variant::operator int() const
{
    return ivalue_;
}

utils::config::variant::operator std::string() const
{
    return svalue_;
}

utils::config::variant::operator utils::geometry() const
{
    return gvalue_;
}


utils::config::config( int argc, char * argv[] )
{
    config::fields_["port"] = 2232;
    config::fields_["window"] = utils::geometry();
    config::fields_["quality"] = 80;
    config::fields_["duration"] = 40;
    config::fields_["verify"] = false;
    
    int c;
    while ((c = getopt (argc, argv, "g:d:p:q:s:t:u:vh")) != -1)
    {
        switch (c)
        {
        case 's':
              config::fields_["shaders"] = optarg;
              break;
        case 't':
              config::fields_["textures"] = optarg;
              break;
        case 'p':
              config::fields_["port"] = ::strtol( optarg, nullptr, 10 );
              break;
        case 'g':
              config::fields_["window"] = utils::geometry( optarg );
              break;
        case 'q':
              config::fields_["quality"] = ::strtol( optarg, nullptr, 10 );
              break;
        case 'd':
              config::fields_["duration"] = ::strtol( optarg, nullptr, 10 );
              break;
        case 'u':
              config::fields_["url"] = optarg;
              break;
        case 'v':
              config::fields_["verify"] = true;
              break;
        case 'h':
        default:
            throw std::runtime_error( "Вывод опций программы" );
        }
    }
}

utils::config::variant &utils::config::operator [](char const *key) const
{
    std::map< std::string, variant >::iterator it = fields_.find( key );
    if( it != fields_.end() )
    {
        return it->second;
    }
    throw std::runtime_error(std::string("config has no key ") + std::string(key) );
}


utils::jpeg_codec::error::error( const std::string &what )
: std::runtime_error( what )
{}

namespace
{
void on_jpeg_error( j_common_ptr cinfo )
{
    char buf[JMSG_LENGTH_MAX];
    ( *( cinfo->err->format_message ) ) ( cinfo, buf );

    throw utils::jpeg_codec::error( buf );
}
}  // namespace

utils::jpeg_codec::jpeg_codec()
{
    cinfo_.err = jpeg_std_error( &jerr_ );
    cinfo_.err->error_exit = on_jpeg_error;
    jpeg_create_decompress( &cinfo_ );
}

utils::jpeg_codec::~jpeg_codec()
{
    jpeg_destroy_decompress( &cinfo_ );
}

bool utils::jpeg_codec::decode( uint8_t const *data, size_t in_size, image *img )
{
    jpeg_mem_src( &cinfo_, data, in_size );
    if( jpeg_read_header( &cinfo_, TRUE ) != 1 )
    {
        return false;
    }
    jpeg_start_decompress( &cinfo_ );

    img->window.width = cinfo_.output_width;
    img->window.height = cinfo_.output_height;
    img->channels = cinfo_.output_components;
    int row_stride = img->window.width * img->channels;
    
    size_t out_size = row_stride * img->window.height;
    if( img->pixels.size() != out_size )
    {
        img->pixels.resize( out_size );
    }

    while( cinfo_.output_scanline < cinfo_.output_height )
    {
        uint8_t *b_array[1] = { img->pixels.data() + cinfo_.output_scanline * row_stride };
        jpeg_read_scanlines( &cinfo_, b_array, 1 );
    }

    jpeg_finish_decompress( &cinfo_ );   
    return true;
}

bool utils::file_exists( char const *filename )
{
    struct stat info;
    if( stat( filename, &info) == -1 )
    {
        return false;
    }
    return (info.st_mode & S_IFMT) == S_IFREG || (info.st_mode & S_IFMT) == S_IFLNK;
}
