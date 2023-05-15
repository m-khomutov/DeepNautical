/* 
 * File:   visitor.h
 * Author: mkh
 *
 * Created on 18 апреля 2023 г., 12:10
 */

#ifndef VISITOR_H
#define VISITOR_H

class antisubmarinefrigate;
class sol;
class water;
class horizon;
class vessel;

class visitor {
public:
    visitor();
    visitor(const visitor& orig) = delete;
    visitor &operator =(const visitor& orig) = delete;
    virtual ~visitor();

    virtual void visit( antisubmarinefrigate * ) = 0;
    virtual void visit( sol * ) = 0;
    virtual void visit( water * ) = 0;
    virtual void visit( horizon * ) = 0;
    virtual void visit( vessel * ) = 0;

private:

};

#endif /* VISITOR_H */

