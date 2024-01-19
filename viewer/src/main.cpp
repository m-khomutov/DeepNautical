/* 
 * File:   main.cpp
 * Author: mkh
 *
 * Created on 25 января 2023 г., 17:24
 */

#include "viewer/baseviewer.h"
#include "utils.h"

#include <getopt.h>
#include <signal.h>
#ifdef QT_CORE_LIB
# include <QApplication>
#else
#include <gtk/gtk.h>
#endif

#include <iostream>
#include <memory>

/*
 * 
 */

namespace
{
std::unique_ptr< baseviewer > main_viewer;

void signal_handler( int s )
{
    main_viewer->onsignal( s );
}

void show_options_and_exit( const char *prog, int rc )
{
    std::cerr << "Запуск: " << prog <<  "[-h] [-u] [-w] [-v]\n\nпрограмма просмотра\n\n";
    std::cerr << "обязательные аргументы:\n";
    std::cerr << "\t-u\turl эмулятора\n"; 
    std::cerr << "Опциональные аргументы:\n";
    std::cerr << "\t-w\tразмеры окна (def. 800x600)\n";
    std::cerr << "\t-v\tвывод оценки задержки (def. false)\n";
    std::cerr << "\t-h\tвывод параметров запуска\n";
    ::exit( rc );   
}
}  // namespace

int main(int argc, char** argv)
{
    try
    {
        utils::config( argc, argv );
        utils::config()["url"];
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
#else
    gtk_init( &argc, &argv );
#endif
    try
    {
        main_viewer = baseviewer::make();
        main_viewer->run();
        return main_viewer->stop();
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
