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
    explicit vessel( const std::vector< std::string > &settings );
    vessel(const vessel& orig) = delete;
    vessel &operator =(const vessel& orig) = delete;
    ~vessel();

private:
    std::string shader_name_;
    std::string obj_name_;
    std::unique_ptr< objreader > objreader_;
    
    std::vector< GLfloat > position_;
    glm::vec3 start_position_ = glm::vec3( 0.0f, 0.0f, 0.0f ); 
    GLfloat x_trajectory_ = 1.0f;
    
    glm::vec2 pitching_{ 0.0f, 0.0f };
    GLfloat pitching_gain_ { 0.0f };
    
    glm::vec3 start_factor_ = glm::vec3( 1.0f, 1.0f, 1.0f );
    GLfloat factor_gain_ = 0.0f;
    glm::vec3 factor_;

private:
    void f_parse_settings( const std::vector< std::string > &settings ) override;
    void f_check_environment() const override;
    char const *f_shader_name() const override;
    void f_initialize() override;
    void f_draw( double currentTime ) override;
    void f_set_model();
};

#endif /* VESSEL_H */
