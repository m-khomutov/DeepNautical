/* 
 * File:   vessel.h
 * Author: mkh
 *
 * Created on 16 февраля 2023 г., 13:11
 */

#ifndef VESSEL_H
#define VESSEL_H

#include "figure.h"
#include "../blender.h"

class vessel: public figure {
public:
    explicit vessel( const std::vector< std::string > &settings );
    vessel(const vessel& orig) = delete;
    vessel &operator =(const vessel& orig) = delete;
    ~vessel();

    void draw();
    const figure::position &position();

private:
    std::unique_ptr< blender::object > object_;
    std::vector< GLfloat > vertices_;
    glm::vec3 factor_;
    GLfloat pitching_angle_ = 0.0f;
    std::shared_ptr< texture > empty_texture_;
    figure::position position_;

private:
    void f_check_environment() const override;
    char const *f_shader_name() const override;
    void f_initialize() override;
    void f_accept( visitor &p, double currentTime ) override;
    void f_set_model();
    bool f_full_trajectory();
    void f_reset();
};

#endif /* VESSEL_H */
