/* 
 * File:   blenderobj.cpp
 * Author: mkh
 * 
 * Created on 1 марта 2023 г., 9:50
 */

#include "blender.h"
#include "utils.h"

#include <QDebug>

#include <cstring>
#include <fstream>

namespace
{
    // читает строку файла .obj, представляющую поверхность (матрица 3х3)
    bool str2face( const std::string &s, NBlender::TMTLfile::face_t &rc )
    {
        uint32_t x[3], y[3], z[3];
        if( sscanf( s.c_str(), "f %u/%u/%u %u/%u/%u %u/%u/%u",
                                  &(x[0]), &(y[0]), &(z[0]),
                                  &(x[1]), &(y[1]), &(z[1]),
                                  &(x[2]), &(y[2]), &(z[2]) ) == 9 )
        {
            for( size_t i(0); i < 3; ++i )
            {
                rc[i].setX( x[i] );
                rc[i].setY( y[i] );
                rc[i].setZ( z[i] );
            }
            return true;
        }
        return false;
    }
}  //namespace

NBlender::TMTLfile::TMTLfile( const char* filename )
{
    std::ifstream ifile( filename );
    if( !ifile.is_open() )
    {
        throw std::runtime_error( std::string(filename) + " error: " + strerror(errno) );
    }

    std::vector< TMaterial > mtls;
    std::string line;
    while( std::getline( ifile, line ) )
    {
        if( line[ 0 ] == 'n' && line.find( "newmtl " ) == 0 ) // получить материал
        {
            mtls.emplace_back( TMaterial() );
            mtls.back().name = line.substr( 7 );
        }
        else if( line[ 0 ] == 'N' )
        {
            switch( line[1] )
            {
            case 's':
                mtls.back().Ns = std::stof( line.substr( 3 ) ); // коэффициент фокусировки блика
                break;
            case 'i':
                mtls.back().Ni = std::stof( line.substr( 3 ) ); // коэффициент преломления
                break;
            }
        }
        else if( line[ 0 ] == 'K' )
        {
            switch( line[1] )
            {
            case 'a':
                NUtils::str2vec( line.substr( 3 ), &mtls.back().Ka ); // коэффициент фонового освещения
                break;
            case 'd':
                NUtils::str2vec( line.substr( 3 ), &mtls.back().Kd ); // коэффициент рассеянного освещения
                break;
            case 's':
                NUtils::str2vec( line.substr( 3 ), &mtls.back().Ks ); // коэффициент бликового освещения
                break;
            case 'e':
                NUtils::str2vec( line.substr( 3 ), &mtls.back().Ke ); // коэффициент зеркального блика
                break;
            }
        }
        else if( line[ 0 ] == 'd' )
        {
            mtls.back().d = std::stof( line.substr( 2 ) ); // коэффициент прозрачности
        }
        else if( line[0] == 'i' && line.find( "illum " ) == 0 )
        {
            mtls.back().illum = std::stof( line.substr( 6 ) ); // номер модели освещенности
        }
        else if( line.find( "map_Kd " ) == 0 ) // текстура объекта
        {
            mtls.back().map_Kd.reset( new QOpenGLTexture( QImage(std::string(std::string(NUtils::TConfig()["objs"]) + "/" + line.substr( 7 )).c_str()) ) );
        }
    }
    for( auto mtl : mtls )
    {
        materials_[mtl.name] = mtl;
    }
}

NBlender::TMTLfile::TMaterial const &NBlender::TMTLfile::operator []( std::string const &name ) const
{
    auto p = materials_.find( name );
    if( p != materials_.end() )
    {
        return p->second;
    }
    throw std::runtime_error( name + " error: no such material" ); 
}



NBlender::TObject::TObject( char const *fname )
{
    std::ifstream ifile( fname );
    if( !ifile.is_open() )
    {
        throw std::runtime_error( std::string(fname) +" error: " +strerror(errno) );
    }
    
    std::unique_ptr< TMTLfile > mltr;

    QVector3D v3;
    QVector2D v2;
    std::string line;
    while( std::getline( ifile, line ) )
    {
        if( line[ 0 ] == 'v' ) // вершины
        {
            switch( line[1] )
            {
            case ' ': // геометрические вершины
                if( NUtils::str2vec( line.substr( 2 ), &v3 ) )
                {
                    vertices_.push_back( v3 );
                }
                break;
            case 't': // текстурные вершины
                if( NUtils::str2vec( line.substr( 3 ), &v2 ) )
                {
                    texels_.push_back( v2 );
                }
                break;
            case 'n': // нормали к геометрическим вершинам
                if( NUtils::str2vec( line.substr( 3 ), &v3 ) )
                {
                    normals_.push_back( v3 );
                }
                break;
            }
        }
        else if( line[ 0 ] == 'f' ) // поверхности
        {
            TMTLfile::face_t face;
            if( str2face( line, face ) && !materials_.empty() )
            {
                materials_.back().faces.push_back( face );
                ++facecount_;
            }
        }
        else if( line.find( "usemtl " ) == 0 ) // название материала
        {
            materials_.push_back( TMTLfile::TMaterial() );
            materials_.back().name = line.substr( 7 );
        }
        else if( line.find( "mtllib " ) == 0 ) //  файл материала
        {
            try
            {
                mltr.reset( new TMTLfile( (std::string(NUtils::TConfig()["objs"]) + "/" + line.substr( 7 )).c_str() ) );
            }
            catch( const std::runtime_error &e )
            {
                qDebug() << "mtl file error: " << e.what();
            }
        }
    }
    if( mltr )
    {
        // заполнить полученные значения материалов
        for( TMTLfile::TMaterial &mtl : materials_ )
        {
            try
            {
                TMTLfile::TMaterial const &m = (*mltr)[mtl.name];
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
                qDebug() << mtl.name.c_str() << " error: no such material in mtlfile";
            }
        }
    }
}

void NBlender::TObject::load_position( std::vector< float > *pos )
{
    // заполнить вектор значениями, которые будут переданы в шейдер для отрисовки
    for( TMTLfile::TMaterial const &mtl : materials_ )
    {
        for( auto f : mtl.faces )
        {
            for( int i(0); i < 3; ++i )
            {
                pos->push_back( vertices_[f[i].x() - 1][0] );
                pos->push_back( vertices_[f[i].x() - 1][1] );
                pos->push_back( vertices_[f[i].x() - 1][2] );
                pos->push_back( texels_[f[i].y() - 1][0] );
                pos->push_back( texels_[f[i].y() - 1][1] );
                pos->push_back( normals_[f[i].z() - 1][0] );
                pos->push_back( normals_[f[i].z() - 1][1] );
                pos->push_back( normals_[f[i].z() - 1][2] );
            }
        }
    }
}
