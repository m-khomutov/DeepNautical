/* 
 * File:   water.h
 * Author: mkh
 *
 * Created on 1 февраля 2023 г., 14:18
 */

#ifndef WATER_H
#define WATER_H

#include "figure.h"

class water: public figure {
public:
    explicit water( const std::vector< std::string > &settings );
    water( water const &orig ) = delete;
    water &operator =( water const &orig ) = delete;
    ~water();

    void draw();
    void set_wake_position( const std::vector< figure::position > &pos );
    
private:
    static const uint32_t resolution = 64;
    GLfloat surface_[6 * resolution * (resolution + 1)]; 
    GLfloat normals_[6 * resolution * (resolution + 1)]; 
    float phase_ { 0.0f };
    std::unique_ptr< texture > air_texture_;    
    std::unique_ptr< texture > foam_texture_;
    std::vector< figure::position > wake_position_;

private:
    void f_check_environment() const override;
    char const *f_shader_name() const override;
    void f_initialize() override;
    void f_accept( visitor &p, double currentTime ) override;

    void f_load_surface( double currentTime );
    GLfloat f_generate_surface( GLfloat x, GLfloat z );
};

#endif /* WATER_H */

