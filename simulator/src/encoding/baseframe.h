/* 
 * File:   baseframe.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 13:27
 */

#ifndef BASEFRAME_H
#define BASEFRAME_H

class baseprotocol;

class baseframe {
public:
    baseframe( int width, int height );
    baseframe( const baseframe& orig ) = delete;
    baseframe &operator =( const baseframe& orig ) = delete;
    virtual ~baseframe();

    void store( int width, int height );
    void load( baseprotocol *, float duration );

    int width() const
    {
        return width_;
    }
    int height() const
    {
        return height_;
    }
    
protected:
    int width_;
    int height_;

private:
    virtual void f_store( int width, int height ) = 0;
    virtual void f_load( baseprotocol * proto, float duration ) = 0;

};

#endif /* BASEFRAME_H */

