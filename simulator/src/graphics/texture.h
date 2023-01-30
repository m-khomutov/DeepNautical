/* 
 * File:   texture.h
 * Author: mkh
 *
 * Created on 30 января 2023 г., 10:46
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <stdexcept>

class texture_error: public std::runtime_error {
public:
    texture_error(const std::string & what);
};

class texture {
public:
    explicit texture( char const *filename );
    texture(const texture& orig) = delete;
    texture &operator =(const texture& orig) = delete;
    ~texture();
    
    void activate() const;

private:
    GLuint id_;
};

#endif /* TEXTURE_H */

