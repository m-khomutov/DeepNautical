/* 
 * File:   screen.h
 * Author: mkh
 *
 * Created on 24 января 2023 г., 9:19
 */

#ifndef SCREEN_H
#define SCREEN_H

#include "shader.h"
#include "../encoding/jpegframe.h"
#include <GLFW/glfw3.h>
#include <memory>


class screen_error: public std::runtime_error {
public:
    screen_error(const std::string & what);
};

class screen {
public:
    explicit screen( baseframe *frame );
    screen(const screen& orig) = delete;
    screen &operator =(const screen &rhs) = delete;
    ~screen();

    void run();
    void stop();

private:
    struct del { void operator()(GLFWwindow * w) { glfwDestroyWindow( w ); glfwTerminate(); } };
    std::unique_ptr< GLFWwindow, del > window_;
    baseframe *frame_;

private:
    static void error_cb( int error, const GLchar * description );
};

#endif /* SCREEN_H */
