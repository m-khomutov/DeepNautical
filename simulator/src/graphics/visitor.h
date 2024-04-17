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
class sol;
class water;
class horizon;
class vessel;
class sparklets;

class visitor {
public:
    visitor();
    visitor(const visitor& orig) = delete;
    visitor &operator =(const visitor& orig) = delete;
    virtual ~visitor();

    virtual void visit( size_t vbo_number, TAntisubmarinefrigate * ) = 0;
    virtual void visit( size_t vbo_number, sol * ) = 0;
    virtual void visit( size_t vbo_number, water * ) = 0;
    virtual void visit( size_t vbo_number, horizon * ) = 0;
    virtual void visit( size_t vbo_number, vessel * ) = 0;
    virtual void visit( size_t vbo_number, sparklets * ) = 0;

private:

};

#endif /* VISITOR_H */

