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
    static bool environment_valid();

    horizon();
    horizon(const horizon& orig) = delete;
    horizon &operator =(const horizon& orig) = delete;
    ~horizon();
    
private:
    GLfloat position_[20] = {    // текстурные координаты
            1.0f,  1.0f, 0.0f,   1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,   1.0f, 1.0f,
           -1.0f, -1.0f, 0.0f,   0.0f, 1.0f,
           -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,
    };
    GLuint indices_[6] = { 0, 1, 3, 1, 2, 3 };
    avi avi_;
    double last_frame_time_ { 0.0f };
    
private:
    char const *f_shader_name() const override;
    void f_initialize() override;
    void f_draw( double current_time ) override;
};

#endif /* HORIZON_H */

