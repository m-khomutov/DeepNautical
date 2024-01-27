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
#include <thread>

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
    service( basescreen *screen, uint16_t port )
    : baseservice( screen, port )
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
    {
        m_scenes.insert( "scene" );
    }

    const std::set< std::string > &scenes() const override
    {
        return m_scenes;
    }
    const std::string &current_scene() const override
    {
        return *m_scenes.begin();
    }
    void set_scene( const std::string &scene ) override
    {}

private:
    size_t m_redrow = 0;

    std::set< std::string > m_scenes;

private:
    void f_run() override
    {}
    void f_stop() override
    {}

    void f_store() override
    {
        uint8_t *buf = frame_->buffer( 800, 600 );

        uint8_t *ptr = buf;
        for( size_t y(0); y < 600; ++y ) {
            for( size_t x(0); x < 800; ++x ) {
                *ptr ++ = x >= m_redrow && x < m_redrow + 10 ? 0xff : 0x00;
                *ptr ++ = 0x00;
                *ptr ++ = 0x00;
            }
        }
        m_redrow = (m_redrow + 1) % 800;
    }

    void f_load( baseprotocol *proto, float duration )
    {
        frame_->load( proto, duration );
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
         std::unique_ptr< basescreen > scr( new screen( new jpegframe( utils::geometry( 800, 600 ), 80, 40 ) ) );

        std::unique_ptr< baseservice > srv( new service( scr.get(), 5555 ) );

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
