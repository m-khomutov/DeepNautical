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

NUtils::TConfig::fields_t NUtils::TConfig::fields_ = NUtils::TConfig::fields_t();

NUtils::TConfig::TVariant::TVariant( int v )
: ivalue_( v )
{}

NUtils::TConfig::TVariant::TVariant( std::string v )
: svalue_( v )
{}


NUtils::TConfig::TVariant::TVariant( NUtils::TGeometry const &v )
: gvalue_( v )
{}

NUtils::TConfig::TVariant::TVariant( NUtils::scene_config_t const &v )
: scvalue_( v )
{}

NUtils::TConfig::TVariant::operator int() const
{
    return ivalue_;
}

NUtils::TConfig::TVariant::operator std::string() const
{
    return svalue_;
}

NUtils::TConfig::TVariant::operator NUtils::TGeometry() const
{
    return gvalue_;
}

NUtils::TConfig::TVariant::operator NUtils::scene_config_t() const
{
    return scvalue_;
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
    NUtils::TGeometry str2conf< NUtils::TGeometry >( char const *line )
    {
        return NUtils::TGeometry( line );
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

NUtils::TConfig::TConfig( int argc, char * argv[] )
{
    // значения папметров по умолчанию, если не предлагаются другие значения
    TConfig::fields_["port"] = 2232;
    TConfig::fields_["window"] = NUtils::TGeometry();
    TConfig::fields_["compress_quality"] = 80;
    TConfig::fields_["frame_duration"] = 40;
    TConfig::fields_["verify"] = false;
    TConfig::fields_["screen_layout"] = scene_config_t();
    TConfig::fields_["bide"] = 2000;
    TConfig::fields_["channel_command_server"] = std::string();

    // параметры командной строки
    int c;
    while ((c = getopt (argc, argv, "d:p:q:s:t:u:l:vb:w:c:o:h")) != -1)
    {
        switch (c)
        {
        case 's': // путь к каталогу шейдеров
              TConfig::fields_["shaders"] = str2conf< std::string >( optarg );
              break;
        case 't': // путь к каталогу текстур
              TConfig::fields_["textures"] = str2conf< std::string >( optarg );
              break;
        case 'o': // путь к каталогу объектных файлов blender
              TConfig::fields_["objs"] = str2conf< std::string >( optarg );
              break;
        case 'p': // сетевой порт получения запросов от абонентов
              TConfig::fields_["port"] = str2conf< int >( optarg );
              break;
        case 'w': // размеры кадра
              TConfig::fields_["window"] = str2conf< NUtils::TGeometry >( optarg );
              break;
        case 'q': // качество сжатия JPEG
              TConfig::fields_["compress_quality"] = str2conf< int >( optarg );
              break;
        case 'd': // длительность кадра
              TConfig::fields_["frame_duration"] = str2conf< int >( optarg );
              break;
        case 'u': // урл до сервиса выдачи видеокадров (абонентская настройка)
              TConfig::fields_["url"] = str2conf< std::string >( optarg );
              break;
        case 'l':
              //TConfig::fields_["screen_layout"] = str2conf< std::string >( optarg );
              break;
        case 'v': // вывод информации о задержке доставки кадра (абонентская настройка)
              TConfig::fields_["verify"] = true;
              break;
        case 'b': // ожидание соединения
              TConfig::fields_["bide"] = str2conf< int >( optarg );
              break;
        case 'c':  // получить конфигурацию из файла)
              f_read_file( optarg );
              break;
        case 'h':
        default:
            throw std::runtime_error( "Вывод опций программы" );
        }
    }
}

NUtils::TConfig::TVariant &NUtils::TConfig::operator [](char const *key) const
{
    std::map< std::string, TVariant >::iterator it = fields_.find( key );
    if( it != fields_.end() )
    {
        return it->second;
    }
    throw std::runtime_error(std::string("config has no key ") + std::string(key) );
}

void NUtils::TConfig::f_read_file( char const *fname )
{
    if( ::strstr( fname, ".json" ) == fname + strlen(fname) - 5 )
    {
        f_read_json( fname );
        return;
    }

    read_config( fname, [this]( const std::string &line ){
        std::string::size_type pos;
        if( (pos = line.find( "shaders=" )) != std::string::npos )
        {
            TConfig::fields_["shaders"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "textures=" )) != std::string::npos )
        {
            TConfig::fields_["textures"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "port=" )) != std::string::npos )
        {
            TConfig::fields_["port"] = str2conf< int >( line.substr( pos + 5 ).c_str() );
        }
        else if( (pos = line.find( "window=" )) != std::string::npos )
        {
            TConfig::fields_["window"] = str2conf< NUtils::TGeometry >( line.substr( pos + 7 ).c_str() );
        }
        else if( (pos = line.find( "jpeg_compress_quality_percent=" )) != std::string::npos )
        {
            TConfig::fields_["compress_quality"] = str2conf< int >( line.substr( pos + 8 ).c_str() );
        }
        else if( (pos = line.find( "frame_duration_msec=" )) != std::string::npos )
        {
            TConfig::fields_["frame_duration"] = str2conf< int >( line.substr( pos + 9 ).c_str() );
        }
        else if( (pos = line.find( "url=" )) != std::string::npos )
        {
            TConfig::fields_["url"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "verify=" )) != std::string::npos )
        {
            TConfig::fields_["verify"] = str2conf< bool >( line.substr( pos +7 ).c_str() );
        }
        else if( (pos = line.find( "objs=" )) != std::string::npos )
        {
            TConfig::fields_["objs"] = str2conf< std::string >( line.c_str() );
        }
        else if( (pos = line.find( "scenes=" )) != std::string::npos )
        {
            TConfig::fields_["scenes"] = str2conf< std::string >( line.c_str() );
        }   
        else if( (pos = line.find( "screen_layout=" )) != std::string::npos )
        {
            //TConfig::fields_["screen_layout"] = str2conf< std::string >( line.c_str() );
        }
    });
}

void NUtils::TConfig::f_read_json( char const *fname )
{
    try
    {
        NJson::TParser p( fname );
        const NJson::TObject &object = p.json();

        // настройки filesystem
        std::string root = std::string(object["filesystem"]["root"]) + "/";
        TConfig::fields_["shaders"] = root + std::string(object["filesystem"]["shaders"]);
        TConfig::fields_["textures"] = root + std::string(object["filesystem"]["textures"]);
        TConfig::fields_["objs"] = root + std::string(object["filesystem"]["blender_objects"]);
        // настройки service
        TConfig::fields_["port"] = object["service"]["port"].toInt();
        TConfig::fields_["compress_quality"] = object["service"]["jpeg_compress_quality_percent"].toInt();
        TConfig::fields_["frame_duration"] = object["service"]["frame_duration_msec"].toInt();

        // настройки сцен
        scene_config_t cfg;

        const NJson::TObject &scenes = object["screen"]["scenes"];
        for( const auto &sc : scenes )
        {
            cfg.emplace( sc.first, sc.second );
        }
        TConfig::fields_["screen_layout"] = cfg;

        // настройки канала комманд
        TConfig::fields_["channel_command_server"] = std::string(object["channel_command_server"]);
    }
    catch( const std::exception &err )
    {
        std::cerr << err.what() << std::endl;
    }
}

NUtils::TJpegCodec::TError::TError( const std::string &what )
: std::runtime_error( what )
{}

namespace
{
void on_jpeg_error( j_common_ptr cinfo )
{
    char buf[JMSG_LENGTH_MAX];
    ( *( cinfo->err->format_message ) ) ( cinfo, buf );

    throw NUtils::TJpegCodec::TError( buf );
}
}  // namespace

NUtils::TJpegCodec::TJpegCodec()
{
    cinfo_.err = jpeg_std_error( &jerr_ );
    cinfo_.err->error_exit = on_jpeg_error;
    jpeg_create_decompress( &cinfo_ );
}

NUtils::TJpegCodec::~TJpegCodec()
{
    jpeg_destroy_decompress( &cinfo_ );
}

bool NUtils::TJpegCodec::decode( char const *filename, TImage *img )
{
    struct stat info;
    if( stat(filename, &info) != 0 )
    {
        throw NUtils::TJpegCodec::TError( strerror( errno ) );
    }

    int fd = open( filename, O_RDONLY );
    if( fd < 0 )
    {
        throw NUtils::TJpegCodec::TError( strerror( errno ) );
    }

    std::vector< uint8_t > jdata( info.st_size );
    size_t rc = ::read( fd, jdata.data(), jdata.size() );
    close(fd);
    if( rc != jdata.size() )
    {
        throw NUtils::TJpegCodec::TError( "not whole file was read" );
    }
    return decode( jdata.data(), jdata.size(), img );
}

bool NUtils::TJpegCodec::decode( uint8_t const *data, size_t in_size, TImage *img )
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
        uint8_t *b_array = { img->pixels.data() + cinfo_.output_scanline * row_stride };
        jpeg_read_scanlines( &cinfo_, &b_array, 1 );
    }

    jpeg_finish_decompress( &cinfo_ );   
    return true;
}

bool NUtils::file_exists( char const *filename )
{
    struct stat info;
    if( stat( filename, &info) == -1 )
    {
        return false;
    }
    return (info.st_mode & S_IFMT) == S_IFREG || (info.st_mode & S_IFMT) == S_IFLNK;
}

void NUtils::read_config( char const *fname, std::function< void( const std::string& ) > foo )
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

bool NUtils::str2key( const std::string &s, std::pair< std::string, std::string > *rc )
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
