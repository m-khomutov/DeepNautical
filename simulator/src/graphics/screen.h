/* 
 * File:   screen.h
 * Author: mkh
 *
 * Created on 24 января 2023 г., 9:19
 */

#ifndef SCREEN_H
#define SCREEN_H

#include "scene.h"
#include "../encoding/jpegframe.h"
//#include <GLFW/glfw3.h>
#include <memory>
#include <QOpenGLWidget>


class screen_error: public std::runtime_error {
public:
    screen_error(const std::string & what);
};

class screen: public QOpenGLWidget {
public:
    explicit screen( baseframe *frame );
    screen(const screen& orig) = delete;
    screen &operator =(const screen &rhs) = delete;
    ~screen();

    void run();
    void stop();

private:
//    struct del { void operator()(GLFWwindow * w) { glfwDestroyWindow( w ); glfwTerminate(); } };
//    std::unique_ptr< GLFWwindow, del > window_;
    baseframe *frame_;
    std::unique_ptr<scene> sc_;
    int update_id_ = { -1 };

    void timerEvent(QTimerEvent*);
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    static void error_cb( int error, const GLchar * description );
};

#endif /* SCREEN_H */
