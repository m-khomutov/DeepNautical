/* сборка командой
 * g++ -o example example_service.cpp -I.. ../../build/kformat/lib/libkformat.a -ljpeg
 * из текущего каталога
 * Открывает сокет tcp и слушает входные запросы на порт 5555.
 * Параллельно не работает. Обслуживает запросы последовательно.
 * Отправляет плееру сгенерированное изображение движущейся вертикальной красной полосы.
 * Для работы нужны 2 класса
 * 1. Класс обслуживания сетевых соединений, производный от baseservice
 * 2. Класс генерации изображений, производный от basescreen
 */

#include <kformat.h>
#include <signal.h>

namespace {
    bool running = true;
    void signal_handler( int s )
    {
        running = false;
    }
}  // namespace


/* Класс обслуживания сетевых соединений */
class service: public baseservice {
public:
    service( basescreen *screen, uint16_t port, uint32_t duration )
    : baseservice( screen, port, duration )
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

/* Класс генерации изображений */
class screen: public basescreen {
public:
    screen( baseframe *frame )
    : basescreen( frame )
    {}

    void run() override
    {}
    void stop() override
    {}

    void store() override
    {
        uint8_t *buf = frame()->buffer( 800, 600 );

        uint8_t *ptr = buf;
        for( size_t y(0); y < 600; ++y ) {
            for( size_t x(0); x < 800; ++x ) {
                *ptr ++ = x >= m_redrow && x < m_redrow + 10 ? 0xff : 0x00;
                *ptr ++ = 0x00;
                *ptr ++ = 0x00;
            }
        }
        frame()->store();
        m_redrow = (m_redrow + 1) % 800;
    }
private:
    size_t m_redrow = 0;
};

int main( int argc, char* argv[] )
{
    signal( SIGHUP,  signal_handler );
    signal( SIGTERM, signal_handler );
    signal( SIGSEGV, signal_handler);
    signal( SIGINT,  signal_handler);

    try
    {
        basescreen *scr( new screen( new jpegframe( utils::geometry( 800, 600 ), 80 ) ) );

        std::unique_ptr< baseservice > srv( new service( scr, 5555, 40 ) );

        srv->run();
        while( running ) {
            std::this_thread::sleep_for( std::chrono::duration( std::chrono::microseconds( 100 )));
            scr->store();
        }

        return srv->stop();
    }
    catch( const std::exception &err )
    {
        std::cerr << "error: " << err.what() << "\n";
    }

    return 0;
}
