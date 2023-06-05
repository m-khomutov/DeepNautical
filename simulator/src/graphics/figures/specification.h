/* 
 * File:   specification.h
 * Author: mkh
 *
 * Created on 1 марта 2023 г., 10:23
 */

#ifndef SPECIFICATION_H
#define SPECIFICATION_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <vector>

class specification {
public:
    specification() = default;
    explicit specification( const std::vector< std::string > &settings );
    specification(const specification& orig) = default;
    specification &operator =(const specification& orig) = default;
    ~specification() = default;

    std::string shader_name;
    std::string texture_name;
    std::string texture_air;
    std::string texture_foam;
    std::string obj_name;
    std::string alpha;
    
    glm::vec3 speed { 0.001f, 0.0f, 0.0f };
    glm::vec3 start_position = glm::vec3( 0.0f, 0.0f, 0.0f ); 
    glm::vec3 course = glm::vec3( 0.0f, 0.0f, 0.0f );
    float trajectory = 1.0f;
    glm::vec2 pitching_range{ 0.0f, 0.0f };
    float pitching = 0.0f;
    glm::vec3 angle_gain { 0.0f, 0.0f, 0.0f };
    
    glm::vec3 start_factor = glm::vec3( 1.0f, 1.0f, 1.0f );
    float factor_gain = 0.0f;

    std::vector< float > viewport;

    glm::vec3 camera_position = glm::vec3( 0.0f, 0.0f, 5.0f ); 
    
    glm::vec3 light_color = glm::vec3( 1.0f, 1.0f, 1.0f ); 
    glm::vec3 light_position = glm::vec3( 0.0f, 1.0f, -3.0f ); 

    // amplitude, velocity
    glm::vec2 wave = { 10.0f, 0.2f };
    glm::vec2 wake = { 20.0f, 2.0f };
    
    glm::vec4 fog_color = {1.0f, 1.0f, 1.0f, 1.0f};
    float fog_density = 0.0f;

    glm::vec2 wake_width = {0.3f, 0.3f};

private:
    void f_read_viewport( const std::string& config );
};

#endif /* SPECIFICATION_H */
