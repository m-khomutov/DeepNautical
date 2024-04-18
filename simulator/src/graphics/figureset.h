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
    using figure_t =  std::vector< std::shared_ptr< TFigure > >;

    figureset();
    figureset(const figureset& orig) = delete;
    figureset &operator =( const figureset &rhs ) = delete;
    ~figureset();
    
    void emplace( TFigure *fig );
    TFigure *back();
    
    void initialize();
    void draw( double currentTime );
    
    void visit( size_t vbo_number, TAntisubmarinefrigate *fig ) override;
    void visit( size_t vbo_number, TSol *fig ) override;
    void visit( size_t vbo_number, water *fig ) override;
    void visit( size_t vbo_number, THorizon *fig ) override;
    void visit( size_t vbo_number, vessel *fig ) override;
    void visit( size_t vbo_number, sparklets *fig ) override;

private:
    std::vector< GLuint > vao_;
    std::vector< GLuint > vbo_;
    std::vector< GLuint > ebo_;
    size_t vbo_count_ { 0 };

    figure_t figures_;
    std::vector< TFigure::TPosition > vessel_positions_;
};

#endif /* FIGURESET_H */
