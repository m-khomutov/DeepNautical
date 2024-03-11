/* сборка командой
 * g++ -o example example_videodev.cpp -I.. ../../build/kformat/lib/libkformat.a -ljpeg
 * из текущего каталога
 * Открывает сокет tcp и слушает входные запросы на порт 5555.
 * Отправляет плееру сгенерированное изображение движущейся вертикальной красной полосы.
 * Для работы нужны 2 класса
 * 1. Класс обслуживания сетевых соединений, производный от baseservice
 * 2. Класс генерации изображений, производный от basescreen
 */

#include <kformat.h>
#include <signal.h>
#include <thread>
#include <iostream>

namespace {

    std::unique_ptr< baseservice > srv;

    void signal_handler( int s )
    {
        if( srv )
        {
            srv->stop();
        }
    }

}  // namespace


/* Класс обслуживания сетевых соединений */
class service: public baseservice {
public:
    service( char const *videodevname, uint16_t port )
    : baseservice( videodevname, port )
    {}

    ~service()
    {}

    void onsignal( int ) override
    {}

private:
    void f_run() override
    {}
    int f_stop() override
    {
        return 0;
    }
};

int main( int argc, char* argv[] )
{
    signal( SIGHUP,  signal_handler );
    signal( SIGTERM, signal_handler );
    signal( SIGSEGV, signal_handler);
    signal( SIGINT,  signal_handler);

    try
    {
        srv.reset( new service( "/dev/video0", 5555 ) );
        srv->run();
    }
    catch( const std::exception &err )
    {
        std::cerr << "error: " << err.what() << "\n";
    }

    return 0;
}
