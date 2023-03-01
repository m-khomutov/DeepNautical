/* 
 * File:   specification.cpp
 * Author: mkh
 * 
 * Created on 1 марта 2023 г., 10:23
 */

#include "specification.h"
#include "../../../../share/utils.h"
#include <iostream>

specification::specification( const std::vector< std::string > &settings )
{
    for( auto s : settings )
    {
        std::pair< std::string, std::string > p;
        if( utils::str2key( s, &p ) )
        {
            if( p.first.find( "shader" ) != std::string::npos )
            {
                shader_name = p.second.substr( 1, p.second.size() - 2 );
            }
            else if( p.first.find( "texture" ) != std::string::npos )
            {
                texture_name = p.second.substr( 1, p.second.size() - 2 );
            }
            else if( p.first.find( "object" ) != std::string::npos )
            {
                obj_name = p.second.substr( 1, p.second.size() - 2 );
            }
            else if( p.first.find( "speed" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &speed ) )
                {
                    std::cerr << "vessel error: invalid speed\n";
                }
            }
            else if( p.first.find( "pitching_range" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &pitching_range ) )
                {
                    std::cerr << "vessel error: invalid pitching\n";
                }
            }
            else if( p.first.find( "start_position" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &start_position ) )
                {
                    std::cerr << "vessel error: invalid start position\n";
                }
            }
            else if( p.first.find( "start_factor" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &start_factor ) )
                {
                    std::cerr << "vessel error: invalid start factor\n";
                }
            }
            else if( p.first.find( "factor_gain" ) != std::string::npos )
            {
                factor_gain = std::stof( p.second );
            }
            else if( p.first.find( "angle_gain" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &angle_gain ) )
                {
                    std::cerr << "vessel error: invalid angle gain\n";
                }
            }
            else if( p.first.find( "trajectory" ) != std::string::npos )
            {
                trajectory = std::stof( p.second );
            }
            else if( p.first.find( "course" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &course ) )
                {
                    std::cerr << "vessel error: invalid course\n";
                }
            }
            else if( p.first.find( "pitching" ) != std::string::npos )
            {
                pitching = std::stof( p.second );
            }
        }
    }
}
