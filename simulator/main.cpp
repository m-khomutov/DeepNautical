/* 
 * File:   main.cpp
 * Author: mkh
 *
 * Created on 23 января 2023 г., 18:31
 */

#include "jpegframe.h"
#include "utils.h"

#include <QApplication>
#include "qservice.h"
#include "qscreen.h"

#include <stdlib.h>
#include <signal.h>
#include <getopt.h>

#include <iostream>

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
    qDebug() << "Запуск: " << prog <<  "[-h] [-s] [-t] [-p] [-q] [-d] [-w] [-o] [-c]\n\nэмулятор устройства\n";
    qDebug() << "обязательные аргументы:";
    qDebug() << "\t-s\tкаталог с шейдерами";
    qDebug() << "\t-t\tкаталог с текстурами";
    qDebug() << "Опциональные аргументы:";
    qDebug() << "\t-p\tпорт прослушки (def. 2232)";
    qDebug() << "\t-w\tразмеры окна (def. 800x600)";
    qDebug() << "\t-q\tкачество сжатия % (def. 80)";
    qDebug() << "\t-d\tдлительность кадра мс (def. 40)";
    qDebug() << "\t-c\tфайл конфигурации";
    qDebug() << "\t-o\tкаталог с объектами blender";
    qDebug() << "\t-h\tвывод параметров запуска";
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
        qDebug() << e.what();
        show_options_and_exit( argv[0], EXIT_FAILURE );
    }

    signal( SIGHUP,  signal_handler );
    signal( SIGTERM, signal_handler );
    signal( SIGSEGV, signal_handler);
    signal( SIGINT,  signal_handler);
    
    try
    {
        QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL, true);
        QApplication a(argc, argv);

        std::setlocale( LC_NUMERIC, "C" );

        std::unique_ptr< TBasescreen > scr( new TQscreen );
        service.reset( new TQService( scr.get(), NUtils::TConfig()["port"] ) );

        service->start_screen();

        service->stop_screen();

        service.reset();

        return (EXIT_SUCCESS);
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
                std::cout << "exception: " << e.what() << "\n";
            }
        }
    }

    return (EXIT_FAILURE);
}
