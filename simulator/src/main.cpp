/* 
 * File:   main.cpp
 * Author: mkh
 *
 * Created on 23 января 2023 г., 18:31
 */

#include <QApplication>

#include "service.h"
#include "../../share/utils.h"

#include <stdlib.h>
#include <signal.h>
#include <getopt.h>

#include <iostream>

/*
 * 
 */

namespace
{
std::unique_ptr< service > main_service;

void signal_handler( int )
{
    main_service->stop();
}

void show_options_and_exit( const char *prog, int rc )
{
    std::cerr << "Запуск: " << prog <<  "[-h] [-s] [-t] [-p] [-g] [-q] [-d]\n\nimitator\n\n";
    std::cerr << "обязательные аргументы:\n";
    std::cerr << "\t-s\tкаталог с шейдерами\n"; 
    std::cerr << "\t-t\tкаталог с текстурами\n"; 
    std::cerr << "Опциональные аргументы:\n";
    std::cerr << "\t-p\tпорт прослушки (def. 2232)\n"; 
    std::cerr << "\t-g\tразмеры окна (def. 800x600)\n";
    std::cerr << "\t-q\tкачество сжатия % (def. 80)\n";
    std::cerr << "\t-d\tдлительность кадра мс (def. 40)\n";
    std::cerr << "\t-h\tshow this help message and exit\n";
    ::exit( rc );   
}
}

int main(int argc, char** argv)
{
    try
    {
        utils::config( argc, argv );
        utils::config()["shaders"];
        utils::config()["textures"];
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
        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
        format.setVersion(3,3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        QSurfaceFormat::setDefaultFormat(format);

        QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL, true);
        QApplication a(argc, argv);
        QApplication::setAttribute(Qt::AA_ForceRasterWidgets, false);

        main_service.reset( new service );
        main_service->run();

        int ret = a.exec();
        main_service->stop();
        return ret;
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
