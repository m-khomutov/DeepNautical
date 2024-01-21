/* сборка командой
 * g++ -o example example.cpp -I. ../build/kformat/lib/libkformat.a
 * из текущего каталога
 * Запуск с одним аргументом - путь к файлу jpeg.
 * Открывает сокет tcp и слушает входные запросы на порт 5555.
 * Параллельно не работает. Обслуживает запросы последовательно.
 * Отправляет плееру содержимое файла jpeg с периодом 40 мс.
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

    void readJPEGContent( const char* filename, std::vector< uint8_t > &data ) {
        FILE *f = fopen( filename, "rb");
        if( !f ) {
            throw std::logic_error( std::string("file error: ") + strerror( errno ) );
        }
        fseek( f, 0, SEEK_END );
        data.resize( ftell( f ) );
        fseek( f, 0, SEEK_SET );
        fread( data.data(), 1, data.size(), f );
        fclose( f );
    }
}  // namespace

int main( int argc, char* argv[] )
{
    if( argc != 2 ) {
        std::cerr << "run: " << argv[0] << " jpeg file\n";
        return 1;
    }

    socklen_t socklen = sizeof (sockaddr_in);
    sockaddr_in dst;
    std::string http_request;
    char buf[1024];

    signal( SIGHUP,  signal_handler );
    signal( SIGTERM, signal_handler );
    signal( SIGSEGV, signal_handler);
    signal( SIGINT,  signal_handler);

    try {
        std::vector< uint8_t > frame;
        readJPEGContent( argv[1], frame );

        Socket sock;
        std::unique_ptr< baseprotocol > proto;


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
                    proto->send_frame( frame.data(), frame.size(), duration );
                    duration += float(t - ts);
                    ts = t;
                }
            }
        }
    }
    catch (std::exception const &e )
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }




    return 0;
}
