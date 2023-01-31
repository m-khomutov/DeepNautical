/* 
 * File:   figure_set.h
 * Author: mkh
 *
 * Created on 24 января 2023 г., 14:52
 */

#ifndef FIGURESET_H
#define FIGURESET_H

#include "figures/figure.h"
#include <vector>

class program;
class scene;

class figureset {
public:
    figureset();
    figureset(const figureset& orig) = delete;
    figureset &operator =( const figureset &rhs ) = delete;
    ~figureset();
    
    void initialize( program &prog );
    void draw( scene &sc, double currentTime );
    
private:
    std::vector< std::shared_ptr< figure > > figures_;
};

#endif /* FIGURESET_H */
