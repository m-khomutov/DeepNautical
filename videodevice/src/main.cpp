#include "service.h"

#include <signal.h>
#include <sys/resource.h>

#include <iostream>
#include <memory>

namespace {

    std::unique_ptr< baseservice > srv;

    void signal_handler( int )
    {
        if( srv )
        {
            srv->stop();
        }
    }

    void daemonize( void )
    {
        if( getppid( ) != 1 )
        {
            if( fork( ) != 0 )
            {
                exit( 0 );
            }
            setsid( );
        }

        struct rlimit flim;
        getrlimit( RLIMIT_NOFILE, &flim );

        for( rlim_t i(0); i < flim.rlim_max; i++ )
        {
            close( i );
        }
    }

    void show_options_and_exit( const char *prog, int rc )
    {
        std::cerr << "Запуск: " << prog <<  "[-d] [-v] [-p] [-h] \n\nсервис выдачи потока от платы видеозахвата в сеть\n\n";
        std::cerr << "Опциональные аргументы:\n";
        std::cerr << "\t-d\tзапускать в режиме демона\n";
        std::cerr << "\t-v\tимя устройства видеозахвата (def. /dev/video0)\n";
        std::cerr << "\t-p\ttcp порт обращения за потоком (def. 5555)\n";
        std::cerr << "\t-h\tвывод параметров запуска\n";
        ::exit( rc );
    }
}  // namespace

int main( int argc, char *argv [] )
{
    bool daemon {false};
    char const *vdevname = "/dev/video0";
    uint16_t port {5555};

    int c;
    while ((c = getopt (argc, argv, "dv:p:h")) != -1)
    {
        switch (c)
        {
        case 'd':
            daemon = true;
            break;
        case 'v':
            vdevname = optarg;
            break;
        case 'p':
            if( !(port = atoi( optarg )) )
            {
                port = 5555;
            }
            break;
        case 'h':
        default:
            show_options_and_exit( argv[0], EXIT_SUCCESS );
        }
    }

    if( daemon )
    {
        daemonize();
    }

    signal( SIGHUP,  signal_handler );
    signal( SIGTERM, signal_handler );
    signal( SIGSEGV, signal_handler);
    signal( SIGINT,  signal_handler);

    try
    {
        srv.reset( new service( vdevname, port ) );
        srv->run();
    }
    catch( const std::exception &err )
    {
        std::cerr << "error: " << err.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
