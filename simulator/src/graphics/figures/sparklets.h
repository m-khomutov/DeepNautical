/* 
 * File:   sparklets.h
 * Author: mkh
 *
 * Created on 21 августа 2023 г., 14:49
 */

#ifndef SPARKLETS_H
#define SPARKLETS_H

#include "figure.h"
#include "../blender.h"

class sparklets: public figure {
public:
    explicit sparklets( const std::vector< std::string > &settings, const glm::vec3 &camera_pos );
    sparklets(const vessel& orig) = delete;
    sparklets &operator =(const sparklets& orig) = delete;
    ~sparklets();

    size_t vbo_count() const override
    {
        return 2;
    }

    void draw( size_t vbo_number );

private:
    std::vector< GLfloat > points_;
    std::vector< GLfloat > colors_;

    GLuint indices_[6] = { 0, 3, 1, 1, 3, 2 };
    double last_frame_time_ { 0.0f };
    std::unique_ptr< texture > air_texture_;    
    
private:
    void f_check_environment() const override;
    char const *f_shader_name() const override;
    void f_initialize( size_t vbo_number ) override;
    void f_accept( size_t vbo_number, visitor &p, double currentTime ) override;

    void f_initialize_layout();
    void f_initialize_sparklets();

    void f_draw_layout();
    void f_draw_sparklets();
    
    void f_set_points();
    void f_reset_points();
};

#endif /* SPARKLETS_H */

