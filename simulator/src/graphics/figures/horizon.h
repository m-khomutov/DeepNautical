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
    explicit horizon( const std::vector< std::string > &settings );
    horizon(const horizon& orig) = delete;
    horizon &operator =(const horizon& orig) = delete;
    ~horizon();
    
private:
    std::string shader_name_;
    std::string texture_name_;
    
    GLfloat position_[20] = {    // текстурные координаты
            1.5f,  1.0f, -1.0f,   1.0f, 0.0f,
            1.5f, -1.0f, -1.0f,   1.0f, 1.0f,
           -1.5f, -1.0f, -1.0f,   0.0f, 1.0f,
           -1.5f,  1.0f, -1.0f,   0.0f, 0.0f,
    };
    GLuint indices_[6] = { 0, 1, 3, 1, 2, 3 };
    std::unique_ptr< avi > avi_;
    double last_frame_time_ { 0.0f };
    
private:
    void f_parse_settings( const std::vector< std::string > &settings ) override;
    void f_check_environment() const override;
    char const *f_shader_name() const override;
    void f_initialize() override;
    void f_draw( double current_time ) override;
};

#endif /* HORIZON_H */

