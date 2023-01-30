/* 
 * File:   screen.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 9:19
 */

#include "screen.h"
#include "scene.h"
#include <iostream>

screen_error::screen_error( const std::string &what )
: std::runtime_error( what )
{
}


screen::screen( baseframe *frame )
: frame_( frame )
{
    glfwSetErrorCallback( screen::error_cb );
    
    if(!glfwInit())
        throw screen_error("glfw init error");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window_.reset( glfwCreateWindow( frame_->width(), frame_->height(), "ПО имитации", nullptr, nullptr ) );
    if( !window_.get() ) {
        glfwTerminate();
        throw screen_error("GLFW window error");
    }
    glfwMakeContextCurrent( window_.get() );
    /*glfwSetKeyCallback( window, key_callback );
    // glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    glfwSetCursorPosCallback( window, mouse_callback );
    glfwSetScrollCallback( window, scroll_callback );*/

    glewExperimental = GL_TRUE;
    if( glewInit() != GLEW_OK ) {
        throw screen_error("GLEW init error");
    }
    int width, height;
    glfwGetFramebufferSize( window_.get(), &width, &height );
    glViewport( 0, 0, width, height );
    glfwSetCursorPos( window_.get(), width / 2, height / 2 );

    glfwSwapInterval(1);
}

screen::~screen()
{
}

void screen::run( const char *shader_dir, const char *texture_dir )
{
    GLint w, h;
    scene sc( shader_dir, texture_dir );
    while( !glfwWindowShouldClose( window_.get() ) )
    {
        glfwGetFramebufferSize( window_.get(), &w, &h );
        sc.display( w, h, glfwGetTime() );
        frame_->store( w, h );
        glfwSwapBuffers( window_.get() );
        glfwPollEvents();
    }
}

void screen::stop()
{
    glfwSetWindowShouldClose( window_.get(), GL_TRUE );
}

void screen::error_cb( int error, const GLchar * description )
{
    throw screen_error( std::string(description) );
}
