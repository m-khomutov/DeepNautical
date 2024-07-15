/* 
 * File:   utils.cpp
 * Author: mkh
 * 
 * Created on 31 января 2023 г., 10:37
 */

#include "utils.h"
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

utils::settings::settings_t utils::settings::settings_ = utils::settings::settings_t();

utils::settings::variant::variant( int v )
    : ivalue_( v )
{}

utils::settings::variant::variant( std::string v )
    : svalue_( v )
{}


utils::settings::variant::variant( utils::winsize const &v )
    : wvalue_( v )
{}

utils::settings::variant::operator int() const
{
    return ivalue_;
}

utils::settings::variant::operator std::string() const
{
    return svalue_;
}

utils::settings::variant::operator utils::winsize() const
{
    return wvalue_;
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
    utils::winsize str2conf< utils::winsize >( char const *line )
    {
        return utils::winsize( line );
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

utils::settings::settings( int argc, char * argv[] )
{
    settings::settings_["port"] = 2232;
    settings::settings_["window"] = utils::winsize();
    settings::settings_["quality"] = 80;
    settings::settings_["duration"] = 40;
    settings::settings_["verify"] = false;
    settings::settings_["scene_count"] = 1;
    settings::settings_["bide"] = 2000;

    int c;
    while ((c = getopt (argc, argv, "d:p:q:s:t:u:vb:w:c:o:h")) != -1)
    {
        switch (c)
        {
        case 's':
              settings::settings_["shaders"] = str2conf< std::string >( optarg );
              break;
        case 't':
              settings::settings_["textures"] = str2conf< std::string >( optarg );
              break;
        case 'o':
              settings::settings_["objs"] = str2conf< std::string >( optarg );
              break;
        case 'p':
              settings::settings_["port"] = str2conf< int >( optarg );
              break;
        case 'w':
              settings::settings_["window"] = str2conf< utils::winsize >( optarg );
              break;
        case 'q':
              settings::settings_["quality"] = str2conf< int >( optarg );
              break;
        case 'd':
              settings::settings_["duration"] = str2conf< int >( optarg );
              break;
        case 'u':
              settings::settings_["url"] = str2conf< std::string >( optarg );
              break;
        case 'v':
              settings::settings_["verify"] = true;
              break;
        case 'b':
              settings::settings_["bide"] = str2conf< int >( optarg );
              break;
        case 'c':
              f_from_file( optarg );
              break;
        case 'h':
        default:
            throw std::runtime_error( "Вывод опций программы" );
        }
    }
}

utils::settings::variant &utils::settings::operator [](char const *key) const
{
    std::map< std::string, variant >::iterator it = settings_.find( key );
    if( it != settings_.end() )
    {
        return it->second;
    }
    throw std::runtime_error(std::string("config has no key ") + std::string(key) );
}

void utils::settings::f_from_file( char const *fname )
{
    read_settings( fname, [this]( const std::string &line ){
        std::string::size_type pos;
        if( (pos = line.find( "shaders=" )) != std::string::npos )
        {
            settings::settings_["shaders"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "textures=" )) != std::string::npos )
        {
            settings::settings_["textures"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "port=" )) != std::string::npos )
        {
            settings::settings_["port"] = str2conf< int >( line.substr( pos + 5 ).c_str() );
        }
        else if( (pos = line.find( "window=" )) != std::string::npos )
        {
            settings::settings_["window"] = str2conf< utils::winsize >( line.substr( pos + 7 ).c_str() );
        }
        else if( (pos = line.find( "quality=" )) != std::string::npos )
        {
            settings::settings_["quality"] = str2conf< int >( line.substr( pos + 8 ).c_str() );
        }
        else if( (pos = line.find( "duration=" )) != std::string::npos )
        {
            settings::settings_["duration"] = str2conf< int >( line.substr( pos + 9 ).c_str() );
        }
        else if( (pos = line.find( "url=" )) != std::string::npos )
        {
            settings::settings_["url"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "verify=" )) != std::string::npos )
        {
            settings::settings_["verify"] = str2conf< bool >( line.substr( pos +7 ).c_str() );
        }
        else if( (pos = line.find( "objs=" )) != std::string::npos )
        {
            settings::settings_["objs"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "scenes=" )) != std::string::npos )
        {
            settings::settings_["scenes"] = str2conf< std::string >( line.c_str() );
        }   
        else if( (pos = line.find( "scene_count=" )) != std::string::npos )
        {
            settings::settings_["scene_count"] = str2conf< int >( line.substr( pos + 12 ).c_str() );
        }
    });
}


utils::jcodec::error::error( const std::string &what )
    : std::runtime_error( what )
{}

namespace
{
void on_jpeg_error( j_common_ptr cinfo )
{
    char buf[JMSG_LENGTH_MAX];
    ( *( cinfo->err->format_message ) ) ( cinfo, buf );
    throw utils::jcodec::error( buf );
}
}  // namespace

utils::jcodec::jcodec()
{
    cinfo_.err = jpeg_std_error( &jerr_ );
    cinfo_.err->error_exit = on_jpeg_error;
    jpeg_create_decompress( &cinfo_ );
}

utils::jcodec::~jcodec()
{
    jpeg_destroy_decompress( &cinfo_ );
}

bool utils::jcodec::decode( char const *filename, frame *dest )
{
    struct stat info;
    if( stat(filename, &info) != 0 )
    {
        throw utils::jcodec::error( strerror( errno ) );
    }

    int fd = open( filename, O_RDONLY );
    if( fd < 0 )
    {
        throw utils::jcodec::error( strerror( errno ) );
    }

    std::vector< uint8_t > jdata( info.st_size );
    size_t rc = ::read( fd, jdata.data(), jdata.size() );
    close(fd);
    if( rc != jdata.size() )
    {
        throw utils::jcodec::error( "not whole file was read" );
    }
    return decode( jdata.data(), jdata.size(), dest );
}

bool utils::jcodec::decode( uint8_t const *data, size_t in_size, frame *dest )
{
    jpeg_mem_src( &cinfo_, data, in_size );
    if( jpeg_read_header( &cinfo_, TRUE ) != 1 )
    {
        return false;
    }
    jpeg_start_decompress( &cinfo_ );

    dest->wsize.width = cinfo_.output_width;
    dest->wsize.height = cinfo_.output_height;
    dest->channels = cinfo_.output_components;
    int row_stride = dest->wsize.width * dest->channels;
    
    size_t out_size = row_stride * dest->wsize.height;
    if( dest->pixels.size() != out_size )
    {
        dest->pixels.resize( out_size );
    }

    while( cinfo_.output_scanline < cinfo_.output_height )
    {
        uint8_t *b_array[1] = { dest->pixels.data() + cinfo_.output_scanline * row_stride };
        jpeg_read_scanlines( &cinfo_, b_array, 1 );
    }

    jpeg_finish_decompress( &cinfo_ );   
    return true;
}

bool utils::exists( char const *filename )
{
    struct stat info;
    if( stat( filename, &info) == -1 )
    {
        return false;
    }
    return (info.st_mode & S_IFMT) == S_IFREG || (info.st_mode & S_IFMT) == S_IFLNK;
}

void utils::read_settings( char const *fname, std::function< void( const std::string& ) > foo )
{
    std::ifstream ifile( fname );
    if( !ifile.is_open() )
    {
        throw std::runtime_error( std::string(fname) + " error: " + strerror(errno) );
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

bool utils::read_header( const std::string &s, std::pair< std::string, std::string > *rc )
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
