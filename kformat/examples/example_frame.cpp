/* сборка командой
 * g++ -o example example_frame.cpp -I.. ../../build/kformat/lib/libkformat.a -ljpeg
 * из текущего каталога
 * Открывает сокет tcp и слушает входные запросы на порт 5555.
 * Параллельно не работает. Обслуживает запросы последовательно.
 * Отправляет плееру сгенерированное изображение движущейся вертикальной красной полосы.
 */

#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>

#include <thread>

#include <kformat.h>

class Socket {
public:
    Socket()
    : m_fd( socket (AF_INET, SOCK_STREAM, IPPROTO_TCP) )
    {
        long yes = 0;
        setsockopt( m_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) );
        sockaddr_in ca = { AF_INET,
                           htons( 5555 ),
                           {INADDR_ANY}
                         };
        if( (bind( m_fd, (sockaddr*)&ca, sizeof(sockaddr_in))) == -1 )
        {
            close( m_fd );
            throw std::logic_error( std::string("bind error: ") + strerror( errno ));
        }

        fcntl( m_fd, F_SETFL, fcntl (m_fd, F_GETFL, 0) | O_NONBLOCK );

        if ( listen( m_fd, 5 ) < 0)
        {
            close( m_fd );
            throw std::logic_error( std::string("listen error: ") + strerror (errno) );
        }
        std::cerr << "[*] listening on port " << ntohs(ca.sin_port) << "\n";
    }

    operator int()
    {
        return m_fd;
    }

    Socket (const Socket&) = delete;
    Socket& operator =(const Socket&) = delete;

    ~Socket() {
        close( m_fd );
        std::cerr << "[*] stop listening\n";
    }

private:
    int m_fd;
};

namespace {
    bool running = true;
    void signal_handler( int s )
    {
        running = false;
    }

    uint64_t now()
    {
        timeval tp;
        gettimeofday( &tp, nullptr );
        return tp.tv_sec * 1000 + tp.tv_usec / 1000;

    }

    void frame_thread( baseframe *frame )
    {
        size_t redrow = 0;
        while( running ) {
            std::this_thread::sleep_for( std::chrono::duration( std::chrono::microseconds( 100 )));
            uint8_t *buf = frame->buffer( 800, 600 );

            uint8_t *ptr = buf;
            for( size_t y(0); y < 600; ++y ) {
                for( size_t x(0); x < 800; ++x ) {
                    *ptr ++ = x >= redrow && x < redrow + 10 ? 0xff : 0x00;
                    *ptr ++ = 0x00;
                    *ptr ++ = 0x00;
                }
            }
            frame->store();
            redrow = (redrow + 1) % 800;
        }
    }
}  // namespace

int main( int argc, char* argv[] )
{
    socklen_t socklen = sizeof (sockaddr_in);
    sockaddr_in dst;
    std::string http_request;
    char buf[1024];

    signal( SIGHUP,  signal_handler );
    signal( SIGTERM, signal_handler );
    signal( SIGSEGV, signal_handler);
    signal( SIGINT,  signal_handler);

    try {
        Socket sock;
        std::unique_ptr< baseprotocol > proto;
        std::unique_ptr< baseframe > jframe( new jpegframe( utils::geometry(800, 600), 80 ) );

        std::thread jthread( frame_thread, jframe.get() );

        timeval tv = { 0, 40 };
        int afd = -1;
        fd_set  rfds;
        uint64_t ts {0};
        float duration = 0.f;

        while( running )
        {
            int mfd = sock;

            FD_ZERO( &rfds );
            FD_SET( sock,  &rfds );
            if( afd != -1 )
            {
                FD_SET( afd, &rfds );
                if( afd > mfd )
                {
                    mfd = afd;
                }
            }

            if( select( mfd + 1, &rfds, NULL, NULL, &tv) > 0 )
            {
                if( FD_ISSET( sock, &rfds) && afd == -1 /* тупой примерчик на одно соединение */ )
                {
                    afd = accept( sock, (sockaddr *)&dst, &socklen );
                    fcntl( afd, F_SETFD, fcntl( afd, F_GETFD, 0) | O_NONBLOCK );
                    inet_ntop( AF_INET, (char *)&(dst.sin_addr), buf, sizeof(sockaddr_in) );
                    std::cerr << "[+] connected with " << buf << ":" << ntohs(dst.sin_port) << "\n";
                }
                if( FD_ISSET( afd, &rfds) )
                {
                    int len;
                    if( (len = recvfrom( afd, buf, sizeof(buf), 0, (sockaddr*)&dst, &socklen)) <= 0)
                    {
                        close( afd );
                        afd = -1;
                        proto.reset();
                        http_request.clear();
                        inet_ntop( AF_INET, (char *)&(dst.sin_addr), buf, sizeof(sockaddr_in) );
                        std::cerr << "[-] connected with " << buf << ":" << ntohs(dst.sin_port) << " closed\n";
                        continue;
                    }
                    if( !proto )
                    {
                        http_request += std::string( (const char*)buf, len );
                        std::cerr << http_request << std::endl;
                        if( http_request.find( "\r\n\r\n" ) != std::string::npos )
                        {
                            if( http_request.find( "GET /stream?proto=flv HTTP/1.1\r\n" ) != std::string::npos )
                            {
                                proto.reset( new flvprotocol( afd ) );
                                ts = now();
                            }
                            http_request.clear();
                        }
                    }
                    if( proto )
                    {
                        proto->on_data( (const uint8_t*)http_request.data(), http_request.size() );
                    }
                }
            }
            if( proto )
            {
                uint64_t t = now();
                if( t - ts > 40 /*период выдачи фрейма 40 мсек */ )
                {
                    jframe->load( proto.get(), duration );
                    duration += float(t - ts);
                    ts = t;
                }
            }
        }

        jthread.join();
    }
    catch (std::exception const &e )
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }




    return 0;
}
