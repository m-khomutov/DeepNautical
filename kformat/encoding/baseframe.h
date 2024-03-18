/* 
 * File:   baseframe.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 13:27
 */

#ifndef BASEFRAME_H
#define BASEFRAME_H

#include "../utils.h"
#include <chrono>

class baseprotocol;

class baseframe {
public:
    using time_point_t = std::chrono::time_point< std::chrono::high_resolution_clock >;

    baseframe( const utils::geometry &geometry, int duration );
    baseframe( const baseframe& orig ) = delete;
    baseframe &operator =( const baseframe& orig ) = delete;
    virtual ~baseframe();

    virtual uint8_t *buffer( int width, int height ) = 0;

    void load( baseprotocol *, float duration );

    int width() const
    {
        return geometry_.width;
    }
    int height() const
    {
        return geometry_.height;
    }
    float duration_passed( time_point_t *ts ) const;
    
protected:
    utils::geometry geometry_;
    std::chrono::milliseconds duration_;
    std::vector< uint8_t > rgb_buffer_;

private:
    virtual void f_load( baseprotocol * proto, float duration ) = 0;

};

#endif /* BASEFRAME_H */

