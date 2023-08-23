/* 
 * File:   figure_set.h
 * Author: mkh
 *
 * Created on 24 января 2023 г., 14:52
 */

#ifndef FIGURESET_H
#define FIGURESET_H

#include "figures/figure.h"
#include "visitor.h"
#include <vector>

class program;
class scene;

class figureset: public visitor {
public:
    using figure_t =  std::vector< std::shared_ptr< figure > >;

    figureset();
    figureset(const figureset& orig) = delete;
    figureset &operator =( const figureset &rhs ) = delete;
    ~figureset();
    
    void emplace( figure *fig );
    figure *back();
    
    void initialize();
    void draw( double currentTime );
    
    void visit( antisubmarinefrigate *fig ) override;
    void visit( sol *fig ) override;
    void visit( water *fig ) override;
    void visit( horizon *fig ) override;
    void visit( vessel *fig ) override;
    void visit( sparklets *fig ) override;

private:
    std::vector< GLuint > vao_;
    std::vector< GLuint > vbo_;
    std::vector< GLuint > ebo_;

    figure_t figures_;
    std::vector< figure::position > vessel_positions_; 
};

#endif /* FIGURESET_H */
