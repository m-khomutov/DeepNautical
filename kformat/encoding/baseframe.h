/* 
 * File:   baseframe.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 13:27
 */

#ifndef BASEFRAME_H
#define BASEFRAME_H

#include "utils.h"

class baseprotocol;

class baseframe {
public:
    baseframe( const utils::geometry &geometry );
    baseframe( const baseframe& orig ) = delete;
    baseframe &operator =( const baseframe& orig ) = delete;
    virtual ~baseframe();

    virtual uint8_t *buffer( int width, int height ) = 0;

    void store();
    void load( baseprotocol *, float duration );

    int width() const
    {
        return geometry_.width;
    }
    int height() const
    {
        return geometry_.height;
    }
    
protected:
    utils::geometry geometry_;
    std::vector< uint8_t > rgb_buffer_;


private:
    virtual void f_store() = 0;
    virtual void f_load( baseprotocol * proto, float duration ) = 0;

};

#endif /* BASEFRAME_H */

