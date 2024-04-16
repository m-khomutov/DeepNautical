/* 
 * File:   main.cpp
 * Author: mkh
 *
 * Created on 25 января 2023 г., 17:24
 */

#include "qviewer.h"
#include "utils.h"

#include <getopt.h>
#include <signal.h>

#include <QApplication>

#include <iostream>
#include <memory>

namespace
{
std::unique_ptr< TBaseviewer > main_viewer; // окно просмотра

// обработать Ctrl+C
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
    qDebug() << "\t-d\tпериод обновления кадра мсек (def. 40)";
    qDebug() << "\t-b\tпериод перед стартом отрисовки ожидания (def. 2000)";
    qDebug() << "\t-h\tвывод параметров запуска";
    ::exit( rc );   
}
}  // namespace

int main(int argc, char** argv)
{
    try
    {
        NUtils::TConfig( argc, argv ); // инициализация конфигурации парамертами запуска
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
        // параметры - период обновления окна и период, после которого начинает крутиться спиннер (мсек)
        main_viewer.reset( new QViewer(NUtils::TConfig()["duration"], NUtils::TConfig()["bide"]) );
        // если надо смотреть сетевую задержку, регистрируем колбэк просмотра
        if( NUtils::TConfig()["verify"] )
        {
            main_viewer->register_verify_callback( []( uint64_t ts ) {
                qDebug() << int64_t(NUtils::now() - ts) << "nsec";
            } );
        }
        main_viewer->start_stream(); // стартуем прием

        return main_viewer->stop_stream(); // программа завершается. Останавливаем прием
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
