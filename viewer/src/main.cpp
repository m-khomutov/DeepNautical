/* 
 * File:   main.cpp
 * Author: mkh
 *
 * Created on 25 января 2023 г., 17:24
 */

#include "viewer.h"
#include "utils.h"

#include <getopt.h>
#include <signal.h>

#include <QApplication>

#include <iostream>
#include <memory>

namespace
{
std::unique_ptr< baseviewer > main_viewer;

void signal_handler( int s )
{
    main_viewer->onsignal( s );
}

void show_options_and_exit( const char *prog, int rc )
{
    qDebug() << "Запуск: " << prog <<  "[-h] [-u] [-w] [-v]\n\nпрограмма просмотра\n";
    qDebug() << "обязательные аргументы:";
    qDebug() << "\t-u\turl эмулятора";
    qDebug() << "Опциональные аргументы:";
    qDebug() << "\t-w\tразмеры окна (def. 800x600)";
    qDebug() << "\t-v\tвывод оценки задержки (def. false)";
    qDebug() << "\t-h\tвывод параметров запуска";
    ::exit( rc );   
}
}  // namespace

int main(int argc, char** argv)
{
    try
    {
        NUtils::TConfig( argc, argv );
        NUtils::TConfig()["url"];
    }
    catch( const std::runtime_error &e )
    {
        qDebug() << e.what();
        show_options_and_exit( argv[0], EXIT_FAILURE );
    }

    signal( SIGHUP,  signal_handler );
    signal( SIGTERM, signal_handler );
    signal( SIGSEGV, signal_handler);
    signal( SIGINT,  signal_handler);

    QApplication a(argc, argv);
    try
    {
        main_viewer.reset( new qviewer );
        main_viewer->run();

        return main_viewer->stop();
    }
    catch( const std::runtime_error &err )
    {
        qDebug() << "error: " << err.what();
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
                qDebug() << "exception: " << e.what();
            }
        }
    }

    return (EXIT_FAILURE);
}
