/* 
 * File:   texture.cpp
 * Author: mkh
 * 
 * Created on 30 января 2023 г., 10:46
 */

#include "texture.h"
#include <SOIL/SOIL.h>
#include <iostream>

texture_error::texture_error( const std::string &what )
: std::runtime_error( what )
{
}

texture::texture( char const *filename )
{
    int w = 0, h = 0;
    unsigned char * image = SOIL_load_image( filename, &w, &h, 0, SOIL_LOAD_RGB );
    if( !(w && h) )
        throw texture_error( std::string("invalid file: ") + std::string(filename) );
    
    glGenTextures( 1, &id_ );
    glBindTexture( GL_TEXTURE_2D, id_ );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap( GL_TEXTURE_2D );   
    glBindTexture( GL_TEXTURE_2D, 0 );
    
    SOIL_free_image_data( image );
}

texture::texture( utils::image &img )
{
    glGenTextures( 1, &id_ );
    glBindTexture( GL_TEXTURE_2D, id_ );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, img.window.width, img.window.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img.pixels.data() );
    glBindTexture( GL_TEXTURE_2D, 0 );
}

texture::~texture()
{
    glDeleteTextures( 1, &id_ );
}

texture &texture::operator =( utils::image &img )
{
    glBindTexture( GL_TEXTURE_2D, id_ );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, img.window.width, img.window.height, GL_RGB, GL_UNSIGNED_BYTE, img.pixels.data() );
    return *this;   
}

void texture::activate() const
{
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, id_ );
}
