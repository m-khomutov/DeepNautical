/* 
 * File:   texture.cpp
 * Author: mkh
 * 
 * Created on 30 января 2023 г., 10:46
 */

#include "texture.h"

texture_error::texture_error( const std::string &what )
: std::runtime_error( what )
{
}


texture::texture( char const *filename, char const *alpha )
{
    GLenum format { GL_RGB };
    
    NUtils::image img;
    NUtils::jpeg_codec codec;
    if( !codec.decode( filename, &img ) )
    {
        throw texture_error( std::string("invalid file: ") + std::string(filename) );
    }
    
    if( alpha )
    {
        NUtils::image a_img;
        if( !codec.decode( alpha, &a_img ) || a_img.pixels.size() * 3 != img.pixels.size() )
        {
            throw texture_error( std::string("invalid alpha file: ") + std::string(alpha) );
        }
        std::vector< uint8_t > tex( img.pixels.size() + a_img.pixels.size() );
        for( size_t i(0); i < a_img.pixels.size(); ++i )
        {
            std::copy( img.pixels.data() + i * 3, img.pixels.data() + i * 3 + 3, tex.data() + i * 4 );
            tex[i * 4 + 3] = a_img.pixels[i];
        }
        std::swap( img.pixels, tex );
        format = GL_RGBA;
    }
    
    glGenTextures( 1, &id_ );
    glBindTexture( GL_TEXTURE_2D, id_ );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, format, img.window.width, img.window.height, 0, format, GL_UNSIGNED_BYTE, img.pixels.data() );
    glGenerateMipmap( GL_TEXTURE_2D );   
    glBindTexture( GL_TEXTURE_2D, 0 );
}

texture::texture( NUtils::image &img )
{
    glGenTextures( 1, &id_ );
    glBindTexture( GL_TEXTURE_2D, id_ );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, img.window.width, img.window.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img.pixels.data() );
    glBindTexture( GL_TEXTURE_2D, 0 );
}

texture::texture( GLuint w, GLuint h, uint8_t value )
{
    std::vector< uint8_t > data( 3 * w * h, value );

    glGenTextures( 1, &id_ );
    glBindTexture( GL_TEXTURE_2D, id_ );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data() );
    glBindTexture( GL_TEXTURE_2D, 0 );
}

texture::~texture()
{
    glDeleteTextures( 1, &id_ );
}

texture &texture::operator =( NUtils::image &img )
{
    glBindTexture( GL_TEXTURE_2D, id_ );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, img.window.width, img.window.height, GL_RGB, GL_UNSIGNED_BYTE, img.pixels.data() );
    return *this;   
}

void texture::activate( uint16_t number ) const
{
    glActiveTexture( number );
    glBindTexture( GL_TEXTURE_2D, id_ );
}
