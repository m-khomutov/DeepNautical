/* 
 * File:   visitor.h
 * Author: mkh
 *
 * Created on 18 апреля 2023 г., 12:10
 */

#ifndef VISITOR_H
#define VISITOR_H

#include <cstddef>

class TAntisubmarinefrigate;
class TSol;
class TWater;
class THorizon;
class TVessel;
class TSparklets;

class visitor {
public:
    visitor();
    visitor(const visitor& orig) = delete;
    visitor &operator =(const visitor& orig) = delete;
    virtual ~visitor();

    virtual void visit( size_t vbo_number, TAntisubmarinefrigate * ) = 0;
    virtual void visit( size_t vbo_number, TSol * ) = 0;
    virtual void visit( size_t vbo_number, TWater * ) = 0;
    virtual void visit( size_t vbo_number, THorizon * ) = 0;
    virtual void visit( size_t vbo_number, TVessel * ) = 0;
    virtual void visit( size_t vbo_number, TSparklets * ) = 0;

private:

};

#endif /* VISITOR_H */

