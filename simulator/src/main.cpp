/* 
 * File:   main.cpp
 * Author: mkh
 *
 * Created on 23 января 2023 г., 18:31
 */

#include "service/baseservice.h"
#include "../../share/utils.h"
#ifdef QT_CORE_LIB
# include <QApplication>
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
std::unique_ptr< baseservice > main_service;

void signal_handler( int s )
{
    main_service->onsignal( s );
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
    
#ifdef QT_CORE_LIB
    QApplication a(argc, argv);
#endif
    try
    {
        main_service.reset( baseservice::make( argc, argv ) );
        main_service->run();

        return main_service->stop();
    }
    catch( const std::runtime_error &err )
    {
        std::cerr << "1. error: " << err.what() << "\n";
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

    main_service.reset();
    return (EXIT_FAILURE);
}
