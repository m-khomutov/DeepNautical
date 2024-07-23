/* 
 * File:   avi.cpp
 * Author: mkh
 * 
 * Created on 12 февраля 2023 г., 14:42
 */

#include "avi.h"
#include <iostream>

TAviTextureError::TAviTextureError( const std::string &what )
: std::runtime_error( what )
{}

namespace
{
    // прочесть из файла sizeof(T) байтов. Вернуть как переменную типа T
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
        throw TAviTextureError( "not enough data to read int" );
    }
    
    // прочесть из файла 4 байта. Вернуть как код FOURCC
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
                if( !isascii( u.buf[ i ] ) ) // код FOURCC - 4 ascii символа
                    throw TAviTextureError( "invalid type: " + std::to_string(u.value) );
            }
            return u.value;
        }
        throw TAviTextureError( "not enough data to read atom" );
    }

    // вернуть строковое представление тега
    std::string atom2str( uint32_t a )
    {
        union {
            uint32_t value;
            char buf[sizeof(value)];
        } u;
        u.value = a;
        return std::string(u.buf, sizeof(u.buf));
    }
}  // namespace


TAviTexture::TFrame::TFrame( FILE *f )
{
    // заполнить поля из файла
    size = intfromfile( f );
    begin = ftell( f );
    fseek( f, size, SEEK_CUR );
}

bool TAviTexture::TFrame::valid( FILE *f ) const
{
    // прочесть 2 байта из начала кадра
    size_t pos = ftell( f );
    fseek( f, begin, SEEK_SET );
    uint8_t buf[2];
    if( fread( (char*)buf, 1, sizeof(buf), f ) != sizeof(buf) )
    {
        throw TAviTextureError( "not enough data to check validness" );
    }
    fseek( f, pos, SEEK_SET );
    
    return buf[0] == 0xff && buf[1] == 0xd8; // должен быть JPEG тег 0xD8
}


std::unique_ptr< TAviTexture::TAtom > TAviTexture::TAtom::make( FILE *f, frame_t *frames )
{
    // код FOURCC тега
    uint32_t t = atomfromfile( f );
    switch( t ) // FOURCC определяет создаваемый тег
    {
    case EType::riff:
    case EType::lst:
        return std::unique_ptr< TAtom >( new Tlist( EType(t), f, frames ) );
    case EType::avih:
        return std::unique_ptr< TAtom >( new Tavih( f, frames ) );
    case EType::strh:
        return std::unique_ptr< TAtom >( new Tstrh( f ) );
    case EType::strf:
        return std::unique_ptr< TAtom >( new Tstrf( f ) );
    }
    
    fseek( f, intfromfile( f ), SEEK_CUR );
    return std::unique_ptr< TAtom >( new TAtom( t ) );
}

std::string TAviTexture::TAtom::to_str() const
{
    return atom2str(fourcc_);
}


TAviTexture::Tavih::Tavih( FILE *f, frame_t *frames )
: TAtom( TAtom::EType::avih )
{
    // заполнить поля из файла
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


TAviTexture::Tstrh::Tstrh( FILE *f )
: TAtom( TAtom::EType::strh )
{
    // заполнить поля из файла
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


TAviTexture::Tstrf::Tstrf( FILE *f )
: TAtom( TAtom::EType::strf )
{
    // заполнить поля из файла
    size_ = intfromfile( f );
    bisize = intfromfile( f ); 
    width = intfromfile( f );
    height = intfromfile( f );
    planes = intfromfile< uint16_t >( f ); 
    bitcount = intfromfile< uint16_t >( f ); 
    compression = intfromfile( f ); 
    sizeimage = intfromfile( f ); 
    xppm = intfromfile( f );
    yppm = intfromfile( f );
    clrused = intfromfile( f ); 
    clrimportant = intfromfile( f ); 
};


TAviTexture::Tlist::Tlist( TAtom::EType type, FILE *f, frame_t *frames )
: TAtom( type )
, type_( type )
{
    // заполнить поля из файла
    size_ = intfromfile( f );
    fourcc_ = atomfromfile( f );
    if( fourcc_ == TAtom::movi )
    {
        while( !feof( f ) )
        {
            // прочесть код FOURCC.
            char buf[4];
            if( fread( buf, 1, sizeof(buf), f ) != sizeof(buf) )
            {
                throw TAviTextureError( "not enough data to get next list atom" );
            }
            // если '00dc' - это кадр
            if( buf[0] == '0' && buf[1] == '0' && buf[2] == 'd' && buf[3] == 'c' )
            {
                frames->emplace_back( TFrame( f ) );
                if( !frames->back().valid( f ) )
                {
                    frames->pop_back();   
                }
                // далее до следующего '0' не особо интересно
                while( !feof( f ) && fgetc( f ) != '0' )
                    ;
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


TAviTexture::TAviTexture( char const *filename )
: ifile_ ( fopen( filename, "r" ) )  // открыть
{
    if( !ifile_ ) {
        throw TAviTextureError( std::string("failed to open ") + filename );
    }
    // получить размер файла
    fseek( ifile_.get(), 0, SEEK_END ); 
    long filesize = ftell( ifile_.get() );
    fseek( ifile_.get(), 0, SEEK_SET ); 

    // прочесть теги из файла
    while( ! feof( ifile_.get() ) )
    {
        try
        {
            std::unique_ptr< TAtom > a( TAtom::make( ifile_.get(), &frames_ ) );
            Tstrh *hdr;
            if( a->fourcc() == TAtom::strh && (hdr = dynamic_cast< Tstrh* >(a.get())) )
            {
                frame_duration_ = 1000.0 / hdr->scale;
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
    // нет кадров - нечем текстурировать
    if( frames_.empty() )
    {
        throw TAviTextureError( std::string(filename) + " has no frames" );
    }
    // настроить итератор по кадрам
    frame_iter_ = frames_.begin();
}

utils::frame &TAviTexture::next_image()
{
    // закольцевать
    if( frame_iter_ == frames_.end() )
    {
        frame_iter_ = frames_.begin();
    }
    // прочесть следующий кадр
    if( jpeg.size() < frame_iter_->size )
    {
        jpeg.resize( frame_iter_->size );
    }
    fseek( ifile_.get(), frame_iter_->begin, SEEK_SET );
    if( fread( (char*)jpeg.data(), 1, frame_iter_->size, ifile_.get() ) == frame_iter_->size )
    {
        try
        {
            // декодировать из JPEG-a
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
