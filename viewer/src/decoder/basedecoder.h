/* 
 * File:   basedecoder.h
 * Author: mkh
 *
 * Created on 26 января 2023 г., 16:40
 */

#ifndef BASEDECODER_H
#define BASEDECODER_H

#include "../../../share/utils.h"
#include <cstdint>
#include <vector>
#include <stdexcept>

class basedecoder_error: public std::runtime_error
{
public:
    basedecoder_error( const std::string & what );
};

class basedecoder_nodata: public std::runtime_error
{
public:
    basedecoder_nodata();
};

struct decframe
{
    std::vector< uint8_t > pixels;
    utils::geometry window;
    int channels { 3 };
    uint32_t timestamp { 0xff };

    decframe( utils::geometry const &win ) : window( win ) {};
};

class basedecoder {
public:
    basedecoder();
    basedecoder(const basedecoder& orig) = delete;
    basedecoder &operator =(const basedecoder& orig) = delete;
    virtual ~basedecoder();

    bool load( decframe *image );
    void store( uint8_t const *frame, size_t size, uint32_t timestamp );
    
protected:
    uint32_t timestamp_ { 0 };
    
private:
    virtual void f_load( decframe *image ) = 0;
    virtual void f_store( uint8_t const *frame, size_t size, uint32_t timestamp ) = 0;

};

#endif /* BASEDECODER_H */

