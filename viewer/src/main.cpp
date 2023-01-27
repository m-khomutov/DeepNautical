/* 
 * File:   main.cpp
 * Author: mkh
 *
 * Created on 25 января 2023 г., 17:24
 */

#include "viewer.h"
#include "../../share/utils.h"

#include <getopt.h>
#include <signal.h>

#include <iostream>
#include <memory>

/*
 * 
 */

namespace
{
std::unique_ptr< viewer > main_viewer;

void signal_handler( int )
{
    main_viewer->stop();
}

void show_options_and_exit( const char *prog, int rc )
{
    std::cerr << "Запуск: " << prog <<  "[-h] [-u] [-g]\n\nimitator\n\n";
    std::cerr << "обязательные аргументы:\n";
    std::cerr << "\t-u\turl симулятора\n"; 
    std::cerr << "Опциональные аргументы:\n";
    std::cerr << "\t-g\tразмеры окна (def. 800x600)\n";
    std::cerr << "\t-h\tshow this help message and exit\n";
    ::exit( rc );   
}
}  // namespace

int main(int argc, char** argv)
{
    const char * url = nullptr;
    utils::geometry w_geom;
    int c;
    while ((c = getopt (argc, argv, "g:u:h")) != -1)
    {
        switch (c)
        {
        case 'u':
              url = optarg;
              break;
        case 'g':
              w_geom = utils::geometry( optarg );
              break;
        case '?':
        case 'h':
        default:
            show_options_and_exit( argv[0], EXIT_SUCCESS );
        }
    }
    if( ! url )
    {
        show_options_and_exit( argv[0], EXIT_FAILURE );
    }

    signal( SIGHUP,  signal_handler );
    signal( SIGTERM, signal_handler );
    signal( SIGSEGV, signal_handler);
    signal( SIGINT,  signal_handler);

    try
    {
        gtk_init( &argc, &argv );

        main_viewer.reset( new viewer( url, w_geom.width, w_geom.height ) );
        main_viewer->run();
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
