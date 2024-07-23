/*!
     \file jpeg.h
     \author mkh
     \date 30 января 2023 г.
     \brief Заголовочный файл класса текстуры JPEG формата.

     Данный файл содержит в себе состояние класса текстуры JPEG формата, объявление его интерфейсов.
 */
#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"
#include <GL/glew.h>
#include <stdexcept>

class TJpegTextureError: public std::runtime_error {
public:
    TJpegTextureError(const std::string & what);
};

class TJpegTexture {
public:
    explicit TJpegTexture( char const *filename, char const *alpha = nullptr );
    explicit TJpegTexture( utils::frame &img );
    TJpegTexture( GLuint w, GLuint h, uint8_t value );
    TJpegTexture( const TJpegTexture& orig ) = delete;

    TJpegTexture &operator =( const TJpegTexture& orig ) = delete;
    ~TJpegTexture();

    TJpegTexture &operator =( utils::frame &img );
    
    void activate( uint16_t number = GL_TEXTURE0 ) const;

private:
    GLuint id_;
};

#endif /* TEXTURE_H */
