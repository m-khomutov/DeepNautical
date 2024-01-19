/* 
 * File:   texture.h
 * Author: mkh
 *
 * Created on 30 января 2023 г., 10:46
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"
#include <GL/glew.h>
#include <stdexcept>

class texture_error: public std::runtime_error {
public:
    texture_error(const std::string & what);
};

class texture {
public:
    explicit texture( char const *filename, char const *alpha = nullptr );
    explicit texture( utils::image &img );
    texture( GLuint w, GLuint h, uint8_t value );
    texture(const texture& orig) = delete;
    texture &operator =(const texture& orig) = delete;
    ~texture();
    
    texture &operator =( utils::image &img );
    
    void activate( uint16_t number = GL_TEXTURE0 ) const;

private:
    GLuint id_;
};

#endif /* TEXTURE_H */
