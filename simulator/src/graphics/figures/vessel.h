/* 
 * File:   vessel.h
 * Author: mkh
 *
 * Created on 16 февраля 2023 г., 13:11
 */

#ifndef VESSEL_H
#define VESSEL_H

#include "figure.h"

class vessel: public figure {
public:
    static bool environment_valid();

    vessel();
    vessel(const vessel& orig) = delete;
    vessel &operator =(const vessel& orig) = delete;
    ~vessel();

private:
    static constexpr glm::vec2 lurch_range = glm::vec2( 15.0f, 22.0f );
    static constexpr GLfloat factor_offset = 0.00005f;
    static constexpr glm::vec3 initial_factor = glm::vec3( 0.05f, 0.05f, 0.05f );
    static constexpr glm::vec3 initial_offset = glm::vec3( -1.2f, 0.21f, 0.0f ); 

    std::vector< GLfloat > position_;
    GLfloat lurch_ { .1f };
    glm::vec3 factor_ { initial_factor };
    size_t facecount_ { 0 };

private:
    char const *f_shader_name() const override;
    void f_initialize() override;
    void f_draw( double currentTime ) override;
    void f_set_model();
};

#endif /* VESSEL_H */
