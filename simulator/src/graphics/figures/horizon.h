/* 
 * File:   horizon.h
 * Author: mkh
 *
 * Created on 13 февраля 2023 г., 17:58
 */

#ifndef HORIZON_H
#define HORIZON_H

#include "figure.h"
#include "../texture/avi.h"

class horizon: public figure {
public:
    explicit horizon( const std::vector< std::string > &settings, const glm::vec3 &camera_pos );
    horizon(const horizon& orig) = delete;
    horizon &operator =(const horizon& orig) = delete;
    ~horizon();

    void draw( size_t vbo_number );
    
private:
    GLuint indices_[6] = { 0, 3, 1, 1, 3, 2 };
    std::unique_ptr< avi > avi_;
    double last_frame_time_ { 0.0f };

private:
    void f_check_environment() const override;
    char const *f_shader_name() const override;
    void f_initialize( size_t vbo_number ) override;
    void f_accept( size_t vbo_number, visitor &p, double current_time ) override;
};

#endif /* HORIZON_H */

