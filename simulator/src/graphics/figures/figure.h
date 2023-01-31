/* 
 * File:   figure.h
 * Author: mkh
 *
 * Created on 31 января 2023 г., 14:35
 */

#ifndef FIGURE_H
#define FIGURE_H

#include "../texture.h"
#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class program;
class scene;

class figure {
public:
    figure();
    figure(const figure& orig) = delete;
    figure &operator=(const figure& orig) = delete;
    virtual ~figure();

    virtual void initialize( program &prog ) = 0;
    virtual void draw( scene &sc, double currentTime ) = 0;

protected:   
    std::unique_ptr< texture > texture_;    
    glm::mat4 rotation_ { glm::rotate(glm::mat4(1.0f), glm::radians( 0.0f ), glm::vec3(1.0f, 0.0f, 0.0f) ) };
    float angle_ = 0.0f;
    float scene_position_ { 0.0f };
    float speed_ { 0.001f };
    float direction_ { -1.0f };
    glm::vec3 offset_ { 0.0f, 0.0f, 0.0f };

private:

};

#endif /* FIGURE_H */

