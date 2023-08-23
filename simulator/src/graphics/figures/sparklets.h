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
    explicit sparklets( const std::vector< std::string > &settings );
    sparklets(const vessel& orig) = delete;
    sparklets &operator =(const sparklets& orig) = delete;
    ~sparklets();

    void draw();

private:
    static const uint32_t resolution = 64;
    std::vector< GLfloat > points_;
    std::vector< GLfloat > colors_;
    std::vector< GLfloat > starts_;
    std::vector< GLfloat > velocities_;
    int width_ { 50 };
    int height_ { 50 };

    GLuint indices_[6] = { 0, 3, 1, 1, 3, 2 };
    double last_frame_time_ { 0.0f };
    std::unique_ptr< texture > air_texture_;    
    
private:
    void f_check_environment() const override;
    char const *f_shader_name() const override;
    void f_initialize() override;
    void f_accept( visitor &p, double currentTime ) override;

    void f_set_points();
    void f_reset_points();
};

#endif /* SPARKLETS_H */

