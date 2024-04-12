/* 
 * File:   main.cpp
 * Author: mkh
 *
 * Created on 23 января 2023 г., 18:31
 */

#include "kformat/service/baseservice.h"
#include "kformat/encoding/jpegframe.h"
#include "kformat/utils.h"
#ifdef QT_CORE_LIB
# include <QApplication>
# include "service/qservice.h"
# include "graphics/screens/qscreen.h"
#else
# include "service/glfwservice.h"
# include "graphics/screens/glfwscreen.h"
#endif

#include <stdlib.h>
#include <signal.h>
#include <getopt.h>

#include <iostream>

/*
 * 
 */

namespace
{

static std::unique_ptr< TBaseservice > service;
void signal_handler( int s )
{
    if( service)
    {
        service->onsignal( s );
    }
}

void show_options_and_exit( const char *prog, int rc )
{
    std::cerr << "Запуск: " << prog <<  "[-h] [-s] [-t] [-p] [-q] [-d] [-w] [-o] [-c]\n\nэмулятор устройства\n\n";
    std::cerr << "обязательные аргументы:\n";
    std::cerr << "\t-s\tкаталог с шейдерами\n"; 
    std::cerr << "\t-t\tкаталог с текстурами\n"; 
    std::cerr << "Опциональные аргументы:\n";
    std::cerr << "\t-p\tпорт прослушки (def. 2232)\n"; 
    std::cerr << "\t-w\tразмеры окна (def. 800x600)\n";
    std::cerr << "\t-q\tкачество сжатия % (def. 80)\n";
    std::cerr << "\t-d\tдлительность кадра мс (def. 40)\n";
    std::cerr << "\t-c\tфайл конфигурации\n";
    std::cerr << "\t-o\tкаталог с объектами blender\n"; 
    std::cerr << "\t-h\tвывод параметров запуска\n";
    ::exit( rc );   
}
}

int main(int argc, char** argv)
{
    try
    {
        NUtils::TConfig( argc, argv );
        NUtils::TConfig()["shaders"];
        NUtils::TConfig()["textures"];
    }
    catch( const std::runtime_error &e )
    {
        std::cerr << e.what() <<std::endl;
        show_options_and_exit( argv[0], EXIT_FAILURE );
    }

    signal( SIGHUP,  signal_handler );
    signal( SIGTERM, signal_handler );
    signal( SIGSEGV, signal_handler);
    signal( SIGINT,  signal_handler);
    
    try
    {
#ifdef QT_CORE_LIB
        QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL, true);
        QApplication a(argc, argv);
        std::unique_ptr< TBasescreen > scr( new qscreen( new TJpegframe( NUtils::TConfig()["window"],
                                                                        NUtils::TConfig()["quality"],
                                                                        NUtils::TConfig()["duration"] ) ) );
        service.reset( new qservice( scr.get(), NUtils::TConfig()["port"] ) );
#else
        std::unique_ptr< TBasescreen > scr ( new glfwscreen( new TJpegframe( NUtils::TConfig()["window"],
                                                                            NUtils::TConfig()["quality"],
                                                                            NUtils::TConfig()["duration"] ) ) );
        service.reset( new glfwservice( scr.get(), NUtils::TConfig()["port"] ) );
#endif

        std::setlocale( LC_NUMERIC,"C" );

        service->start_screen();

        return service->stop_screen();
    }
    catch( const std::runtime_error &err )
    {
        std::cerr << "error: " << err.what() << "\n";
    }
    catch( ...)
    {
        std::exception_ptr ep = std::current_exception();
        if( ep )
        {
            try
            {
                std::rethrow_exception( ep );
            }
            catch( const std::exception& e )
            {
                std::cout << "exception: " << e.what() << "\n";
            }
        }
    }

    return (EXIT_FAILURE);
}
