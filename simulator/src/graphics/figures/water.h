/* 
 * File:   water.h
 * Author: mkh
 *
 * Created on 1 февраля 2023 г., 14:18
 */

#ifndef WATER_H
#define WATER_H

#include "figure.h"

class water: public TFigure {
public:
    explicit water( const std::vector< std::string > &settings, const glm::vec3 &camera_pos );
    water( water const &orig ) = delete;
    water &operator =( water const &orig ) = delete;
    ~water();

    void draw( size_t vbo_number );
    void set_wake_position( const std::vector< TFigure::TPosition > &pos );
    
private:
    static const uint32_t resolution = 64;
    GLfloat surface_[8 * resolution * (resolution + 1)];
    GLfloat normals_[6 * resolution * (resolution + 1)];
    float phase_ { 0.0f };
    std::unique_ptr< texture > air_texture_;    
    std::unique_ptr< texture > foam_texture_;
    std::vector< TFigure::TPosition > wake_position_;

private:
    void f_check_environment() const override;
    char const *f_shader_name() const override;
    void f_initialize( size_t vbo_number ) override;
    void f_accept( size_t vbo_number, visitor &p, double currentTime ) override;

    void f_load_surface( double currentTime );
    GLfloat f_generate_surface( GLfloat x, GLfloat z, GLfloat *in_wake = nullptr );
};

#endif /* WATER_H */

