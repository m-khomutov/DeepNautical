/* 
 * File:   main.cpp
 * Author: mkh
 *
 * Created on 23 января 2023 г., 18:31
 */

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
    const char * shader_dir = nullptr;
    const char * texture_dir = nullptr;
    uint16_t b_port { 2232 };
    utils::geometry w_geom;
    int quality = 80;
    int duration = 40;
    int c;
    while ((c = getopt (argc, argv, "g:d:p:q:s:t:h")) != -1)
    {
        switch (c)
        {
        case 's':
              shader_dir = optarg;
              break;
        case 't':
              texture_dir = optarg;
              break;
        case 'p':
              b_port = ::strtol( optarg, nullptr, 10 );
              break;
        case 'g':
              w_geom = utils::geometry( optarg );
              break;
        case 'q':
              quality = ::strtol( optarg, nullptr, 10 );
              break;
        case 'd':
              duration = ::strtol( optarg, nullptr, 10 );
              break;
        case '?':
        case 'h':
        default:
            show_options_and_exit( argv[0], EXIT_SUCCESS );
        }
    }
    if( !(shader_dir && texture_dir) )
    {
        show_options_and_exit( argv[0], EXIT_FAILURE );
    }

    signal( SIGHUP,  signal_handler );
    signal( SIGTERM, signal_handler );
    signal( SIGSEGV, signal_handler);
    signal( SIGINT,  signal_handler);

    try
    {
        main_service.reset( new service( b_port,
                                         w_geom.width,
                                         w_geom.height,
                                         quality,
                                         duration ) );
        main_service->run( shader_dir, texture_dir );
    	return (EXIT_SUCCESS);
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
