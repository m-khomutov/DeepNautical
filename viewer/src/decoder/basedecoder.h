/* 
 * File:   basedecoder.h
 * Author: mkh
 *
 * Created on 26 января 2023 г., 16:40
 */

#ifndef BASEDECODER_H
#define BASEDECODER_H

#include "utils.h"
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

class basedecoder {
public:
    basedecoder();
    basedecoder(const basedecoder& orig) = delete;
    basedecoder &operator =(const basedecoder& orig) = delete;
    virtual ~basedecoder();

    bool load( utils::image *img );
    void store( uint8_t const *frame, size_t size, uint64_t timestamp );
    
protected:
    uint64_t timestamp_ { 0ul };
    
private:
    virtual void f_load( utils::image *img ) = 0;
    virtual void f_store( uint8_t const *frame, size_t size, uint64_t timestamp ) = 0;

};

#endif /* BASEDECODER_H */
