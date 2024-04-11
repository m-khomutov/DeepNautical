/* 
 * File:   screen.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 9:19
 */

#include "glfwscreen.h"

glfwscreen::glfwscreen( TBaseframe *frame )
: openglscreen( frame )
{
    glfwSetErrorCallback( glfwscreen::error_cb );
    
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

glfwscreen::~glfwscreen()
{}

void glfwscreen::f_run()
{
    for( int i(0); i < utils::config()["scene_count"]; ++i )
    {
        if( scene_iter_ == scenes_.end() )
        {
            throw screen_error("not enough scenes to start");
        }
        sc_.emplace_back( new scene( std::string(utils::config()["scenes"]) + "/" + *scene_iter_ + ".scn" ) );
        ++scene_iter_;
    }

    viewes_ = 0;
    for( auto &s : sc_ )
    {
        viewes_ += s->cameras();
    }

    glfwSetWindowSize( window_.get(), viewes_ * frame_->width(), frame_->height() );
    GLint w, h;
    while( !glfwWindowShouldClose( window_.get() ) )
    {
        f_exec_command();

        glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glfwGetFramebufferSize( window_.get(), &w, &h );
        for( size_t s(0), view(0); s < sc_.size(); ++s )
        {
            for( size_t v( 0 ); v < sc_[s]->cameras(); ++v )
            {
                glViewport( view * frame_->width(), 0, frame_->width(), h );
                sc_[s]->display( v, frame_->width(), h, glfwGetTime() * 1000 );
                ++view;
            }
        }

        basescreen::store();

        glfwSwapBuffers( window_.get() );
        glfwPollEvents();
    }
}

void glfwscreen::f_stop()
{
    glfwSetWindowShouldClose( window_.get(), GL_TRUE );
}

void glfwscreen::error_cb( int error, const GLchar * description )
{
    throw screen_error( std::string(description) );
}
