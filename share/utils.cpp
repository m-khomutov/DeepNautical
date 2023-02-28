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
#include <string.h>
#include <dirent.h> 
#include <string.h> 
#include <fstream>

utils::config::fields_t utils::config::fields_ = utils::config::fields_t();

utils::config::variant::variant( int v )
: ivalue_( v )
{
}

utils::config::variant::variant( std::string v )
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


namespace
{
    template< typename T >
    T str2conf( char const *line )
    {
        return std::string( line );
    }
    template<>
    std::string str2conf< std::string >( char const *line )
    {
        char const *from = strchr( line, '"' );
        if( from )
        {
            char const *to = strchr( from + 1, '"' );
            if( to )
            {
                return std::string( from + 1, to - (from + 1) );
            }
        }
        return std::string(line);
    }
    template<>
    int str2conf< int >( char const *line )
    {
        return ::strtol( line, nullptr, 10 );
    }
    template<>
    utils::geometry str2conf< utils::geometry >( char const *line )
    {
        return utils::geometry( line );
    }
    template<>
    bool str2conf< bool >( char const *line )
    {
        if( strstr( line, "True" ) == line || strstr( line, "true" ) == line )
        {
            return true;
        }
        return false;
    }
}  // namespace

utils::config::config( int argc, char * argv[] )
{
    config::fields_["port"] = 2232;
    config::fields_["window"] = utils::geometry();
    config::fields_["quality"] = 80;
    config::fields_["duration"] = 40;
    config::fields_["verify"] = false;
    
    int c;
    while ((c = getopt (argc, argv, "d:p:q:s:t:u:vw:c:o:h")) != -1)
    {
        switch (c)
        {
        case 's':
              config::fields_["shaders"] = str2conf< std::string >( optarg );
              break;
        case 't':
              config::fields_["textures"] = str2conf< std::string >( optarg );
              break;
        case 'o':
              config::fields_["objs"] = str2conf< std::string >( optarg );
              break;
        case 'p':
              config::fields_["port"] = str2conf< int >( optarg );
              break;
        case 'w':
              config::fields_["window"] = str2conf< utils::geometry >( optarg );
              break;
        case 'q':
              config::fields_["quality"] = str2conf< int >( optarg );
              break;
        case 'd':
              config::fields_["duration"] = str2conf< int >( optarg );
              break;
        case 'u':
              config::fields_["url"] = str2conf< std::string >( optarg );
              break;
        case 'v':
              config::fields_["verify"] = true;
              break;
        case 'c':
              f_read_file( optarg );
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

void utils::config::f_read_file( char const *fname )
{
    read_config( fname, [this]( const std::string &line ){
        std::string::size_type pos;
        if( (pos = line.find( "shaders=" )) != std::string::npos )
        {
            config::fields_["shaders"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "textures=" )) != std::string::npos )
        {
            config::fields_["textures"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "port=" )) != std::string::npos )
        {
            config::fields_["port"] = str2conf< int >( line.substr( pos + 5 ).c_str() );
        }
        else if( (pos = line.find( "window=" )) != std::string::npos )
        {
            config::fields_["window"] = str2conf< utils::geometry >( line.substr( pos + 7 ).c_str() );
        }
        else if( (pos = line.find( "quality=" )) != std::string::npos )
        {
            config::fields_["quality"] = str2conf< int >( line.substr( pos + 8 ).c_str() );
        }
        else if( (pos = line.find( "duration=" )) != std::string::npos )
        {
            config::fields_["duration"] = str2conf< int >( line.substr( pos + 9 ).c_str() );
        }
        else if( (pos = line.find( "url=" )) != std::string::npos )
        {
            config::fields_["url"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "verify=" )) != std::string::npos )
        {
            config::fields_["verify"] = str2conf< bool >( line.substr( pos +7 ).c_str() );
        }
        else if( (pos = line.find( "objs=" )) != std::string::npos )
        {
            config::fields_["objs"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "scenes=" )) != std::string::npos )
        {
            config::fields_["scenes"] = str2conf< std::string >( line.c_str() );
        }   
    });
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

void utils::read_directory( const std::string &path,
                            char const *filter, 
                            std::function< void( const std::string& ) > foo )
{
    DIR *dir;
    struct dirent *entry;
    if( !(dir = opendir( path.c_str() ) ) )
        throw std::runtime_error( path + std::string(" error: ") + std::string(strerror( errno ) ) );

    while( (entry = readdir(dir))  )
    {
        if( entry->d_type == DT_REG && strstr( entry->d_name, filter ) )
        {
            foo( path + "/" + entry->d_name );
        }
    }
    closedir( dir );
}

void utils::read_config( char const *fname, std::function< void( const std::string& ) > foo )
{
    std::ifstream ifile( fname );
    if( !ifile.is_open() )
    {
        std::cerr<< fname << " error: " << strerror(errno) << std::endl;
        return;
    }
    std::string line;
    while( std::getline( ifile, line ) )
    {
        if( line.size() < 3 || line[0] == '#' )
        {
            continue;
        }
        foo( line );
    }
    ifile.close();
}

bool utils::str2key( const std::string &s, std::pair< std::string, std::string > *rc )
{
    size_t pos;
    if( (pos = s.find( "=" )) != std::string::npos )
    {
        rc->first = s.substr(0, pos );
        rc->second = s.substr( pos + 1 );
        return true;
    }
    return false;
}
