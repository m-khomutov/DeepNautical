/* 
 * File:   blenderobj.cpp
 * Author: mkh
 * 
 * Created on 1 марта 2023 г., 9:50
 */

#include "blender.h"
#include "kformat/utils.h"
#include <cstring>
#include <fstream>

namespace
{
   
    bool str2face( const std::string &s, blender::mtlfile::face_t *rc )
    {
        return sscanf( s.c_str(), "f %u/%u/%u %u/%u/%u %u/%u/%u",
                       &((*rc)[0][0]), &((*rc)[0][1]), &((*rc)[0][2]),
                       &((*rc)[1][0]), &((*rc)[1][1]), &((*rc)[1][2]),
                       &((*rc)[2][0]), &((*rc)[2][1]), &((*rc)[2][2]) ) == 9;
    }
}  //namespace

blender::mtlfile::mtlfile( const char* filename )
{
    std::ifstream ifile( filename );
    if( !ifile.is_open() )
    {
        throw std::runtime_error( std::string(filename) + " error: " + strerror(errno) );
    }

    std::vector< material > mtls;
    std::string line;
    while( std::getline( ifile, line ) )
    {
        if( line[ 0 ] == 'n' && line.find( "newmtl " ) == 0 )
        {
            mtls.emplace_back( material() );
            mtls.back().name = line.substr( 7 );
        }
        else if( line[ 0 ] == 'N' )
        {
            switch( line[1] )
            {
            case 's':
                mtls.back().Ns = std::stof( line.substr( 3 ) );
                break;
            case 'i':
                mtls.back().Ni = std::stof( line.substr( 3 ) );
                break;
            }
        }
        else if( line[ 0 ] == 'K' )
        {
            switch( line[1] )
            {
            case 'a':
                NUtils::str2vec( line.substr( 3 ), &mtls.back().Ka );
                break;
            case 'd':
                NUtils::str2vec( line.substr( 3 ), &mtls.back().Kd );
                break;
            case 's':
                NUtils::str2vec( line.substr( 3 ), &mtls.back().Ks );
                break;
            case 'e':
                NUtils::str2vec( line.substr( 3 ), &mtls.back().Ke );
                break;
            }
        }
        else if( line[ 0 ] == 'd' )
        {
            mtls.back().d = std::stof( line.substr( 2 ) );
        }
        else if( line[0] == 'i' && line.find( "illum " ) == 0 )
        {
            mtls.back().illum = std::stof( line.substr( 6 ) );
        }
        else if( line.find( "map_Kd " ) == 0 )
        {
            mtls.back().map_Kd.reset( new texture( (std::string(NUtils::config()["objs"]) + "/" + line.substr( 7 )).c_str() ) );
        }
    }
    for( auto mtl : mtls )
    {
        materials_[mtl.name] = mtl;
    }
}

blender::mtlfile::material const &blender::mtlfile::operator []( std::string const &name ) const
{
    auto p = materials_.find( name );
    if( p != materials_.end() )
    {
        return p->second;
    }
    throw std::runtime_error( name + " error: no such material" ); 
}



blender::object::object( char const *fname )
{
    std::ifstream ifile( fname );
    if( !ifile.is_open() )
    {
        throw std::runtime_error( std::string(fname) +" error: " +strerror(errno) );
    }
    
    std::unique_ptr< mtlfile > mltr;

    glm::vec3 v3;
    glm::vec2 v2;
    std::string line;
    while( std::getline( ifile, line ) )
    {
        if( line[ 0 ] == 'v' )
        {
            switch( line[1] )
            {
            case ' ':
                if( NUtils::str2vec( line.substr( 2 ), &v3 ) )
                {
                    vertices_.push_back( v3 );
                }
                break;
            case 't':
                if( NUtils::str2vec( line.substr( 3 ), &v2 ) )
                {
                    texels_.push_back( v2 );
                }
                break;
            case 'n':
                if( NUtils::str2vec( line.substr( 3 ), &v3 ) )
                {
                    normals_.push_back( v3 );
                }
                break;
            }
        }
        else if( line[ 0 ] == 'f' )
        {
            mtlfile::face_t face;
            if( str2face( line, &face ) && !materials_.empty() )
            {
                materials_.back().faces.push_back( face );
                ++facecount_;
            }
        }
        else if( line.find( "usemtl " ) == 0 )
        {
            materials_.push_back( mtlfile::material() );
            materials_.back().name = line.substr( 7 );
        }
        else if( line.find( "mtllib " ) == 0 )
        {
            try
            {
                mltr.reset( new mtlfile( (std::string(NUtils::config()["objs"]) + "/" + line.substr( 7 )).c_str() ) );
            }
            catch( const std::runtime_error &e )
            {
                std::cerr << "mtl file error: " <<e.what() << std::endl;
            }
        }
    }
    if( mltr )
    {
        for( mtlfile::material &mtl : materials_ )
        {
            try
            {
                mtlfile::material const &m = (*mltr)[mtl.name];
                mtl.Ns = m.Ns;
                mtl.Ka = m.Ka;
                mtl.Kd = m.Kd;
                mtl.Ks = m.Ks;
                mtl.Ke = m.Ke;
                mtl.Ni = m.Ni;
                mtl.d = m.d;
                mtl.illum = m.illum;
                mtl.map_Kd = m.map_Kd;
            }
            catch( const std::runtime_error &e )
            {
                std::cerr << mtl.name << " error: no such material in mtlfile\n";
            }
        }
    }
}

void blender::object::load_position( std::vector< float > *pos )
{
    for( mtlfile::material const &mtl : materials_ )
    {
        for( auto f : mtl.faces )
        {
            for( int i(0); i < 3; ++i )
            {
                pos->push_back( vertices_[f[i].x - 1][0] );
                pos->push_back( vertices_[f[i].x - 1][1] );
                pos->push_back( vertices_[f[i].x - 1][2] );
                pos->push_back( texels_[f[i].y - 1][0] );
                pos->push_back( texels_[f[i].y - 1][1] );
                pos->push_back( normals_[f[i].z - 1][0] );
                pos->push_back( normals_[f[i].z - 1][1] );
                pos->push_back( normals_[f[i].z - 1][2] );
            }
        }
    }
}
