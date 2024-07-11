/* 
 * File: utils.cpp
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

utils::settings::settings_t utils::settings::fields_ = utils::settings::settings_t();

utils::settings::var::var( int v )
    : i_value_( v )
{}

utils::settings::var::var( std::string v )
    : s_value_( v )
{}


utils::settings::var::var( utils::winsize const &v )
    : wsz_value_( v )
{}

utils::settings::var::var( utils::set_scenecfg_t const &v )
    : ssc_value_( v )
{}

utils::settings::var::operator int() const
{
    return i_value_;
}

utils::settings::var::operator std::string() const
{
    return s_value_;
}

utils::settings::var::operator utils::winsize() const
{
    return wsz_value_;
}

utils::settings::var::operator utils::set_scenecfg_t() const
{
    return ssc_value_;
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
    settings::fields_["port"] = 2232;
    settings::fields_["window"] = utils::winsize();
    settings::fields_["compress_quality"] = 80;
    settings::fields_["frame_duration"] = 40;
    settings::fields_["verify"] = false;
    settings::fields_["screen_layout"] = set_scenecfg_t();
    settings::fields_["bide"] = 2000;
    settings::fields_["channel_command_server"] = std::string();

    int c;
    while ((c = getopt (argc, argv, "d:p:q:s:t:u:l:vb:w:c:o:h")) != -1)
    {
        switch (c)
        {
        case 's':
              settings::fields_["shaders"] = str2conf< std::string >( optarg );
              break;
        case 't':
              settings::fields_["textures"] = str2conf< std::string >( optarg );
              break;
        case 'o':
              settings::fields_["objs"] = str2conf< std::string >( optarg );
              break;
        case 'p':
              settings::fields_["port"] = str2conf< int >( optarg );
              break;
        case 'w':
              settings::fields_["window"] = str2conf< utils::winsize >( optarg );
              break;
        case 'q':
              settings::fields_["compress_quality"] = str2conf< int >( optarg );
              break;
        case 'd':
              settings::fields_["frame_duration"] = str2conf< int >( optarg );
              break;
        case 'u':
              settings::fields_["url"] = str2conf< std::string >( optarg );
              break;
        case 'v':
              settings::fields_["verify"] = true;
              break;
        case 'b':
              settings::fields_["bide"] = str2conf< int >( optarg );
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

utils::settings::var &utils::settings::operator [](char const *key) const
{
    std::map< std::string, var >::iterator it = fields_.find( key );
    if( it != fields_.end() )
    {
        return it->second;
    }
    throw std::runtime_error(std::string("config has no key ") + std::string(key) );
}

void utils::settings::f_read_file( char const *fname )
{
    if( ::strstr( fname, ".json" ) == fname + strlen(fname) - 5 )
    {
        f_read_jsfile( fname );
        return;
    }

    read_config( fname, [this]( const std::string &line ){
        std::string::size_type pos;
        if( (pos = line.find( "shaders=" )) != std::string::npos )
        {
            settings::fields_["shaders"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "textures=" )) != std::string::npos )
        {
            settings::fields_["textures"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "port=" )) != std::string::npos )
        {
            settings::fields_["port"] = str2conf< int >( line.substr( pos + 5 ).c_str() );
        }
        else if( (pos = line.find( "window=" )) != std::string::npos )
        {
            settings::fields_["window"] = str2conf< utils::winsize >( line.substr( pos + 7 ).c_str() );
        }
        else if( (pos = line.find( "jpeg_compress_quality_percent=" )) != std::string::npos )
        {
            settings::fields_["compress_quality"] = str2conf< int >( line.substr( pos + 8 ).c_str() );
        }
        else if( (pos = line.find( "frame_duration_msec=" )) != std::string::npos )
        {
            settings::fields_["frame_duration"] = str2conf< int >( line.substr( pos + 9 ).c_str() );
        }
        else if( (pos = line.find( "url=" )) != std::string::npos )
        {
            settings::fields_["url"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "verify=" )) != std::string::npos )
        {
            settings::fields_["verify"] = str2conf< bool >( line.substr( pos +7 ).c_str() );
        }
        else if( (pos = line.find( "objs=" )) != std::string::npos )
        {
            settings::fields_["objs"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "scenes=" )) != std::string::npos )
        {
            settings::fields_["scenes"] = str2conf< std::string >( line.c_str() );
        }   
    });
}

void utils::settings::f_read_jsfile( char const *fname )
{
    try
    {
        js::parser p( fname );
        const js::object &object = p.root();

        // настройки filesystem
        std::string root = std::string(object["filesystem"]["root"]) + "/";
        settings::fields_["shaders"] = root + std::string(object["filesystem"]["shaders"]);
        settings::fields_["textures"] = root + std::string(object["filesystem"]["textures"]);
        settings::fields_["objs"] = root + std::string(object["filesystem"]["blender_objects"]);
        // настройки service
        settings::fields_["port"] = object["service"]["port"].toInt();
        settings::fields_["compress_quality"] = object["service"]["jpeg_compress_quality_percent"].toInt();
        settings::fields_["frame_duration"] = object["service"]["frame_duration_msec"].toInt();

        // настройки сцен
        set_scenecfg_t cfg;

        const js::object &scenes = object["screen"]["scenes"];
        for( const auto &sc : scenes )
        {
            cfg.emplace( sc.first, sc.second );
        }
        settings::fields_["screen_layout"] = cfg;

        // настройки канала комманд
        settings::fields_["channel_command_server"] = std::string(object["channel_command_server"]);
    }
    catch( const std::exception &err )
    {
        std::cerr << err.what() << std::endl;
    }
}

utils::jpegcodec::error::error( const std::string &what )
    : std::runtime_error( what )
{}

namespace
{
void on_jpeg_error( j_common_ptr cinfo )
{
    char buf[JMSG_LENGTH_MAX];
    ( *( cinfo->err->format_message ) ) ( cinfo, buf );

    throw utils::jpegcodec::error( buf );
}
}  // namespace

utils::jpegcodec::jpegcodec()
{
    cinfo_.err = jpeg_std_error( &jerr_ );
    cinfo_.err->error_exit = on_jpeg_error;
    jpeg_create_decompress( &cinfo_ );
}

utils::jpegcodec::~jpegcodec()
{
    jpeg_destroy_decompress( &cinfo_ );
}

bool utils::jpegcodec::decode( char const *filename, frame *fr )
{
    struct stat info;
    if( stat(filename, &info) != 0 )
    {
        throw utils::jpegcodec::error( strerror( errno ) );
    }

    int fd = open( filename, O_RDONLY );
    if( fd < 0 )
    {
        throw utils::jpegcodec::error( strerror( errno ) );
    }

    std::vector< uint8_t > jdata( info.st_size );
    size_t rc = ::read( fd, jdata.data(), jdata.size() );
    close(fd);
    if( rc != jdata.size() )
    {
        throw utils::jpegcodec::error( "not whole file was read" );
    }
    return decode( jdata.data(), jdata.size(), fr );
}

bool utils::jpegcodec::decode( uint8_t const *data, size_t in_size, frame *fr )
{
    jpeg_mem_src( &cinfo_, data, in_size );
    if( jpeg_read_header( &cinfo_, TRUE ) != 1 )
    {
        return false;
    }
    jpeg_start_decompress( &cinfo_ );

    fr->window.width = cinfo_.output_width;
    fr->window.height = cinfo_.output_height;
    switch( cinfo_.output_components ) {
    case 1:
        fr->channels = frame::color::Monochrome;
        break;
    case 3:
        fr->channels = frame::color::RGB;
        break;
    }

    int row_stride = fr->window.width * int(fr->channels);

    size_t out_size = row_stride * fr->window.height;
    if( fr->pixels.size() != out_size )
    {
        fr->pixels.resize( out_size );
    }

    while( cinfo_.output_scanline < cinfo_.output_height )
    {
        uint8_t *b_array = { fr->pixels.data() + cinfo_.output_scanline * row_stride };
        jpeg_read_scanlines( &cinfo_, &b_array, 1 );
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

void utils::read_config( char const *fname, std::function< void( const std::string& ) > foo )
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
