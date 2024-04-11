/* 
 * File:   screen.h
 * Author: mkh
 *
 * Created on 24 января 2023 г., 9:19
 */

#ifndef GLFWSCREEN_H
#define GLFWSCREEN_H

#include "../shader.h"
#include "openglscreen.h"
#include <GLFW/glfw3.h>
#include <memory>


class glfwscreen: public openglscreen {
public:
    explicit glfwscreen( TBaseframe *frame );
    glfwscreen(const glfwscreen& orig) = delete;
    glfwscreen &operator =(const glfwscreen &rhs) = delete;
    ~glfwscreen();

private:
    struct del { void operator()(GLFWwindow * w) { glfwDestroyWindow( w ); glfwTerminate(); } };
    std::unique_ptr< GLFWwindow, del > window_;

private:
    static void error_cb( int error, const GLchar * description );

    void f_run() override;
    void f_stop() override;
};

#endif /* GLFWSCREEN_H */
