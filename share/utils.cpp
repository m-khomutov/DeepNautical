/* 
 * File:   utils.cpp
 * Author: mkh
 * 
 * Created on 31 января 2023 г., 10:37
 */

#include "utils.h"
#include <getopt.h>
#include <iostream>

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


namespace
{
    std::string options( char const *prog ) 
    {
        return std::string("Запуск: ") + prog + "[-h] [-s] [-t] [-p] [-g] [-q] [-d]\n\nimitator\n\n" +
               "обязательные аргументы:\n" +
               "\t-s\tкаталог с шейдерами\n" + 
               "\t-t\tкаталог с текстурами\n" + 
               "Опциональные аргументы:\n" +
               "\t-p\tпорт прослушки (def. 2232)\n" + 
               "\t-g\tразмеры окна (def. 800x600)\n" +
               "\t-q\tкачество сжатия % (def. 80)\n" +
               "\t-d\tдлительность кадра мс (def. 40)\n" +
               "\t-h\tshow this help message and exit\n";	
    };
}

utils::config::config( int argc, char * argv[] )
{
    config::fields_["port"] = 2232;
    config::fields_["window"] = utils::geometry();
    config::fields_["quality"] = 80;
    config::fields_["duration"] = 40;
    
    int c;
    while ((c = getopt (argc, argv, "g:d:p:q:s:t:h")) != -1)
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
        case '?':
        case 'h':
        default:
            throw std::runtime_error( options( argv[0] ) );
        }
    }
    operator[]("shaders");
    operator[]("textures");
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
