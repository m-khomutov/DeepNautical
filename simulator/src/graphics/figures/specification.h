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
#include <string>
#include <vector>

class specification {
public:
    explicit specification( const std::vector< std::string > &settings );
    specification(const specification& orig) = delete;
    specification &operator =(const specification& orig) = delete;
    ~specification() = default;

    std::string shader_name;
    std::string texture_name;
    std::string obj_name;
    
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

private:
    void f_read_viewport( const std::string& config );
};

#endif /* SPECIFICATION_H */
