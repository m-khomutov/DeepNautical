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
class water;
class THorizon;
class vessel;
class sparklets;

class visitor {
public:
    visitor();
    visitor(const visitor& orig) = delete;
    visitor &operator =(const visitor& orig) = delete;
    virtual ~visitor();

    virtual void visit( size_t vbo_number, TAntisubmarinefrigate * ) = 0;
    virtual void visit( size_t vbo_number, TSol * ) = 0;
    virtual void visit( size_t vbo_number, water * ) = 0;
    virtual void visit( size_t vbo_number, THorizon * ) = 0;
    virtual void visit( size_t vbo_number, vessel * ) = 0;
    virtual void visit( size_t vbo_number, sparklets * ) = 0;

private:

};

#endif /* VISITOR_H */

