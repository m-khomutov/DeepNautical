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
            else if( p.first.find( "alpha" ) != std::string::npos )
            {
                alpha = p.second.substr( 1, p.second.size() - 2 );
            }
            else if( p.first.find( "air" ) != std::string::npos )
            {
                texture_air = p.second.substr( 1, p.second.size() - 2 );
            }
            else if( p.first.find( "foam" ) != std::string::npos )
            {
                texture_foam = p.second.substr( 1, p.second.size() - 2 );
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
            else if( p.first.find( "viewport" ) != std::string::npos )
            {
                f_read_viewport( p.second );
            }
            else if( p.first.find( "light_color" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &light_color ) )
                {
                    std::cerr << "light error: invalid light color\n";
                }
            }
            else if( p.first.find( "light_position" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &light_position ) )
                {
                    std::cerr << "light error: invalid light position\n";
                }
            }
            else if( p.first.find( "wave" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &wave ) )
                {
                    std::cerr << "wave error\n";
                }
            }
            else if( p.first.find( "wake" ) != std::string::npos )
            {
                if( p.first.find( "wake_width" ) != std::string::npos )
                {
                    if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &wake_width ) )
                    {
                        std::cerr << "vessel error: invalid wake parameters\n";
                    }
                }
                else if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &wake ) )
                {
                    std::cerr << "wake error\n";
                }
            }
            else if( p.first.find( "camera_position" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &camera_position ) )
                {
                    std::cerr << "camera position error\n";
                }
            }
            else if( p.first.find( "fog_color" ) != std::string::npos )
            {
                if( ! utils::str2vec( p.second.substr( 1, p.second.size() - 2 ), &fog_color ) )
                {
                    std::cerr << "fog color error\n";
                }
            }
            else if( p.first.find( "fog_density" ) != std::string::npos )
            {
                fog_density = std::stof( p.second );
            }
            else if( p.first.find( "surge" ) != std::string::npos )
            {
                f_read_surge( p.second );
            }
        }
    }
}

void specification::f_read_viewport( const std::string& config )
{
    std::vector< float > rc( 20 );
    rc[3] = 0.0f; rc[4] = 0.0f;
    rc[8] = 1.0f; rc[9] = 0.0f;
    rc[13] = 1.0f; rc[14] = 1.0f;
    rc[18] = 0.0f; rc[19] = 1.0f;

    size_t off { 0 };
    glm::vec3 point;
    size_t p1 = 2, p2 = config.find("} {");
    while( p2 != std::string::npos )
    {
        if( ! utils::str2vec( config.substr( p1, p2 - p1 ), &point ) )
        {
            std::cerr << "invalid viewport: " << config << "\n";
            return;
        }
        for( int i(0); i <3; ++i ) rc[off++] = point[i];
	off += 2;
        p1 = p2 + 3;
        p2 = config.find("} {", p1);
    }
    p2 = config.find("}}", p1 );
    if( p2 != std::string::npos )
    {
        if( ! utils::str2vec( config.substr( p1, p2 - p1 ), &point ) )
        {
            std::cerr << "invalid viewport: " << config << "\n";
            return;
        }
        for( int i(0); i <3; ++i ) rc[off++] = point[i];
    }
    std::swap( viewport, rc );
}

void specification::f_read_surge( const std::string& config )
{
    size_t off { 0 };
    size_t p1 = 2, p2 = config.find("} {");
    while( p2 != std::string::npos )
    {
        if( ! utils::str2vec( config.substr( p1, p2 - p1 ), &surge[off] ) )
        {
            std::cerr << "invalid surge: " << config << "\n";
            return;
        }
        if( ++off > 1 )
        {
            return;
        }
        p1 = p2 + 3;
        p2 = config.find("} {", p1);
    }
    if( off > 1 )
    {
        return;
    }
    p2 = config.find("}}", p1 );
    if( p2 != std::string::npos )
    {
        if( ! utils::str2vec( config.substr( p1, p2 - p1 ), &surge[off] ) )
        {
            std::cerr << "invalid viewport: " << config << "\n";
            return;
        }
    }
}
