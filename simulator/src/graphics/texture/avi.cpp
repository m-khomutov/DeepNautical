/* 
 * File:   avi.cpp
 * Author: mkh
 * 
 * Created on 12 февраля 2023 г., 14:42
 */

#include "avi.h"
#include <iostream>

avi_error::avi_error( const std::string &what )
: std::runtime_error( what )
{
}

namespace
{
    template< typename T = uint32_t >
    T intfromfile( FILE *f )
    {
        union {
            T value;
            char buf[sizeof(T)];
        } u;
        if( fread( u.buf, 1, sizeof(u.buf), f ) == sizeof(u.buf) )
        {
            return u.value;
        }
        throw avi_error( "not enough data to read int" );
    }
    
    uint32_t atomfromfile( FILE *f )
    {
        union {
            uint32_t value;
            char buf[sizeof(value)];
        } u;
        if( fread( u.buf, 1, sizeof(u.buf), f ) == sizeof(u.buf) )
        {
            for( size_t i(0); i < sizeof(u.buf); ++i )
            {
                if( !isascii( u.buf[ i ] ) )
                    throw avi_error( "invalid type: " + std::to_string(u.value) );
            }
            return u.value;
        }
        throw avi_error( "not enough data to read atom" );
    }
    std::string atom2str( uint32_t a )
    {
        union {
            uint32_t value;
            char buf[sizeof(value)];
        } u;
        u.value = a;
        return std::string(u.buf, sizeof(u.buf));
    }
    /*std::ostream &operator <<( std::ostream& out, const avi::atom &a )
    {
        out << a.to_str();
        return out;
    }*/
}  // namespace


avi::frame::frame( FILE *f )
{
    size = intfromfile( f );
    begin = ftell( f );
    fseek( f, size, SEEK_CUR );
}

bool avi::frame::valid( FILE *f ) const
{
    size_t pos = ftell( f );
    fseek( f, begin, SEEK_SET );
    uint8_t buf[2];
    if( fread( (char*)buf, 1, sizeof(buf), f ) != sizeof(buf) )
    {
        throw avi_error( "not enough data to check validness" );
    }
    fseek( f, pos, SEEK_SET );
    
    return buf[0] == 0xff && buf[1] == 0xd8;
}


std::unique_ptr< avi::atom > avi::atom::make( FILE *f, frame_t *frames )
{
    uint32_t t = atomfromfile( f );
    switch( t )
    {
    case atom::type::riff:
    case atom::type::lst:
        return std::unique_ptr< atom >( new avi::list( atom::type(t), f, frames ) );
    case atom::avih:
        return std::unique_ptr< avi::atom >( new avi::avih( f, frames ) );
    case atom::strh:
        return std::unique_ptr< avi::atom >( new avi::strh( f ) );
    case atom::strf:
        return std::unique_ptr< avi::atom >( new avi::strf( f ) );
    }
    
    fseek( f, intfromfile( f ), SEEK_CUR );
    return std::unique_ptr< avi::atom >( new atom( t ) );
}

std::string avi::atom::to_str() const
{
    return atom2str(fourcc_);
}

avi::avih::avih( FILE *f, frame_t *frames )
: atom( atom::type::avih )
{
    size_ = intfromfile( f );
    framerate_mcs = intfromfile( f );
    transferrate = intfromfile( f );
    padding = intfromfile( f );
    flags = intfromfile( f );
    frame_count = intfromfile( f );
    initial_frames = intfromfile( f );
    streams = intfromfile( f );
    suggested_bufsize = intfromfile( f );
    width = intfromfile( f );
    height = intfromfile( f );
    fseek( f, 4 * sizeof( uint32_t ), SEEK_CUR );
    frames->resize( 0 );
    frames->reserve( frame_count + 10 );
}


avi::strh::strh( FILE *f )
: atom( atom::type::strh )
{
    size_ = intfromfile( f );
    fcc_type = intfromfile( f );
    fcc_handler = intfromfile( f );
    flags = intfromfile( f );
    priority = intfromfile( f );
    language = intfromfile( f );
    initial_frames = intfromfile( f );
    scale = intfromfile( f );
    rate = intfromfile( f );
    start = intfromfile( f );
    length = intfromfile( f );
    suggested_bufsize = intfromfile( f );
    quality = intfromfile( f );
    samplesize = intfromfile( f );
    for( size_t i(0); i < sizeof(frame) / sizeof(uint32_t); ++i )
    {
        frame[ i ] = fgetc( f );
    }
}

avi::strf::strf( FILE *f )
: atom( atom::type::strf )
{
    size_ = intfromfile( f );
    bisize = intfromfile( f ); 
    width = intfromfile( f ); //LONG
    height = intfromfile( f ); //LONG
    planes = intfromfile< uint16_t >( f ); 
    bitcount = intfromfile< uint16_t >( f ); 
    compression = intfromfile( f ); 
    sizeimage = intfromfile( f ); 
    xppm = intfromfile( f ); //LONG
    yppm = intfromfile( f ); //LONG
    clrused = intfromfile( f ); 
    clrimportant = intfromfile( f ); 
};

avi::list::list( atom::type type, FILE *f, frame_t *frames )
: atom( type )
, type_( type )
{
    size_ = intfromfile( f );
    fourcc_ = atomfromfile( f );
    if( fourcc_ == atom::movi )
    {
        while( !feof( f ) )
        {
            char buf[4];
            if( fread( buf, 1, sizeof(buf), f ) != sizeof(buf) )
            {
                throw avi_error( "not enough data to get next list atom" );
	    }
            if( buf[0] == '0' && buf[1] == '0' && buf[2] == 'd' && buf[3] == 'c' )
            {
                frames->emplace_back( frame( f ) );
                if( ! frames->back().valid( f ) )
                {
                    frames->pop_back();   
                }
                while( !feof( f ) && fgetc( f ) != '0' ) ;
                fseek( f, -1, SEEK_CUR );
                continue;
            }
            if( buf[0] == 'm' && buf[1] == 'o' && buf[2] == 'v' && buf[3] == 'i' )
            {
                continue;
            }
            break;
        }
    }
}


avi::avi( char const *filename )
: ifile_ ( fopen( filename, "r" ) )
{
    if( !ifile_ ) {
        throw avi_error( std::string("failed to open ") + filename );
    }
    fseek( ifile_.get(), 0, SEEK_END ); 
    long filesize = ftell( ifile_.get() );
    fseek( ifile_.get(), 0, SEEK_SET ); 

    while( ! feof( ifile_.get() ) )
    {
        try
        {
            std::unique_ptr< atom > a( atom::make( ifile_.get(), &frames_ ) );
            strh *hdr;
            if( a->fourcc() == atom::strh && (hdr = dynamic_cast< strh* >(a.get())) )
            {
                frame_duration_ = 1000.0d / hdr->scale;
            }
        }
        catch( const std::runtime_error &e)
        {
            std::cerr <<e.what() <<std::endl;
        }
        if( ftell( ifile_.get() ) >= filesize )
        {
           break;
        }
    }
    if( frames_.empty() )
    {
        throw avi_error( std::string(filename) + " has no frames" );
    }
    frame_iter_ = frames_.begin();
}

avi::~avi()
{
}

NUtils::TImage &avi::next_image()
{
    if( frame_iter_ == frames_.end() )
    {
        frame_iter_ = frames_.begin();
    }
    if( jpeg.size() < frame_iter_->size )
    {
        jpeg.resize( frame_iter_->size );
    }
    fseek( ifile_.get(), frame_iter_->begin, SEEK_SET );
    if( fread( (char*)jpeg.data(), 1, frame_iter_->size, ifile_.get() ) == frame_iter_->size )
    {
        try
        {
            codec_.decode( jpeg.data(), frame_iter_->size, &image_ );
            ++ frame_iter_;
        }
        catch( const std::runtime_error &err )
        {
            std::cerr << err.what() << std::endl;
            frame_iter_ = frames_.begin();
        }
    }
    else
    {
        std::cerr << "failed to read full image\n";
        ++ frame_iter_;
    }
    return image_; 
}
