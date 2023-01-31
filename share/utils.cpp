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
