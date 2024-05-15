/*!
     \file:   utils.h
     \author: mkh
     \date 26 января 2023 г.
     \brief Заголовочный файл вспомогательных классов и функций.

     Данный файл содержит в себе объявления вспомогательных классов и функций.
 */
#ifndef UTILS_H
#define UTILS_H

#include "json.h"

#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <dirent.h>
#include <sys/time.h>
extern "C"
{
 #include <jpeglib.h>
 #include <jerror.h>
}
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <iterator>
#include <sstream>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

namespace NUtils
{
/*!
     \class TGeometry
     \brief Структура размеров геометрического объекта.

     Содержит размеры объекта (ширина, высота). Позволяет получить эти размеры из строки конфигурации.
 */
struct TGeometry
{
    //! ширина объекта
    int width = 800;
    //! высота объекта
    int height = 600;

    /*!
       \brief Конструктор структуры по умолчанию. Необходим для хранения объекта геометрии в контейнерах стандартной библиотеки.
     */
    TGeometry() = default;
    /*!
       \brief Конструктор структуры, заполняющие её состояние непосредственно
       \param w ширина объекта
       \param h высота объекта
     */
    TGeometry( int w, int h )
    : width( w )
    , height( h )
    {}

    /*!
       \brief Конструктор структуры, заполняющие её состояние из строки конфигурации
       \param from строка корфигурации, содержащая размеры объекта в виде widthxheight
     */
    TGeometry( const char * from )
    {
        char *ptr;
        int w = strtol( from, &ptr, 10 );
        if( w && ptr )
        {
            int h = strtol( ptr + 1, nullptr, 10 );
            if( h )
            {
                width = w;
                height = h;
            }
        }
    }
};


/*!
     \class TSceneConfig
     \brief Структура настроек сцены.

     Содержит название сцены, геометрию (ширина, высота) и местоположение на экране (x, y).
 */
struct TSceneConfig
{
    //! название сцены
    std::string name;
    //! геометрия сцены
    TGeometry size;
    //! местоположение по горизонтали
    int x = 0;
    //! местоположение по вертикали
    int y = 0;

    /*!
       \brief Конструктор структуры по умолчанию. Необходим для хранения объекта геометрии в контейнерах стандартной библиотеки.
     */
    TSceneConfig() = default;
    /*!
       \brief Конструктор структуры, заполняющие её состояние из JSON-строки конфигурации
       \param from строка корфигурации, содержащая размеры объекта в виде widthxheight
     */
    TSceneConfig( const std::string &n, const NJson::TObject &obj )
    : name( n )
    , size( obj["size"]["width"].toInt(), obj["size"]["height"].toInt() )
    , x( obj["position"]["x"].toInt() )
    , y( obj["position"]["y"].toInt() )
    {}

    bool operator <( const TSceneConfig &other ) const
    {
        if( x == other.x )
        {
            return y < other.y;
        }
        return x < other.x;
    }
};
//! алиас множества конфигураций сцен
using scene_config_t = std::set< TSceneConfig >;

/*!
    \class TSafeguard
    \brief RAII объект, обеспечивающий потокобезопасность объекта хранения

    Владеет объектом хранения. Предоставляет доступ к объекту только под мьютексом.
 */
template< typename T >
class TSafeguard {
public:
    /*!
       \class TGuard
       \brief RTTI Класс потокобезопасного доступа к объекту

       При создании получает указатель на охраняемый объект и мьютекс, закрывающий доступ.
       Закрывает мьютекс, переданный по указателю и освобождает его при удалении собственного объекта.
       Таким образом, пока не удален объект этого класса, охраняемый объект закрыт мьютексом
     */
    class TGuard {
    public:
        /*!
           \brief оператор доступа к охраняемому объекту по ссылке
           \return ссылка на охраняемый объект
         */
        T& operator *()
        {
            return *value_;
        }
        /*!
           \brief оператор доступа к охраняемому объекту по указателю
           \return указатель на охраняемый объект
         */
        T* operator ->()
        {
            return value_;
        }

        /*!
           \brief Конструктор перемещения класса потокобезопасного доступа к объекту
           \param rhs перемещаемый объект.
         */
        TGuard( TGuard && rhs )
        : m_( std::move( rhs.m_ ) )
        , value_( rhs.value_ )
        {}

    private:
        /*!
         * \brief Конструктор настройки состояния объекта класса. Вызывается объетктом класса TSafeguard
         * \param m Указатель на защищающий мьютекс. Закрвывается здесь. Открывается при удалении объекта класса
         * \param v Указатель на охраняемый объект
         */
        TGuard( std::mutex *m, T *v )
        : m_( m, []( std::mutex *m_p ){ m_p->unlock(); } )
        , value_( v )
        {
            m_->lock();
        }

    private:
        /*!
           \brief указатель на закрывающий мьютекс.

           Им владеет объект класса TSafeguard, создающий объекты этого класса.
           Задача этого класса - закрыть мьютекс на создании и освободить на удалении своего объекта
          */
        std::shared_ptr< std::mutex > m_;
        //! указатель на охраняемый объект
        T *value_;
        //! Класс, создающий объекты данного класса должен иметь доступ к его состоянию
        friend class TSafeguard;
    };

public:
    /*!
     * \brief Конструктор класса. Создает охраняемый объект
     */
    TSafeguard()
    : value_( new T )
    {}
    /*!
        \brief Запрещенный конструктор копии.
        \param orig Копируемый объект
      */
    TSafeguard( const TSafeguard& orig ) = delete;
    /*!
        \brief Запрещенный оператор присваивания.
        \param orig Копируемый объект
        \return Собственный объект
      */
    TSafeguard &operator =( const TSafeguard& orig ) = delete;

    /*!
       \brief создает объект класса TGuard, закрывающий охраняемый объект
       \return объект класса TGuard, закрывающий охраняемый объект
     */
    TGuard value_guard()
    {
        return TGuard( &mutex_, value_.get() );
    }

private:
    //! защищающий мьютекс. Управляется в объектах класса TGuard
    std::mutex mutex_;
    //! Умный указатель на охраняемый объект
    std::unique_ptr< T > value_;
};


/*!
    \class TScopedThread
    \brief RAII объект, запускающий поток выполнения

     Запускает поток выполнения для объекта, имеющего методы старта (start_listening_network) и останова(stop_listening_network).
     В случае генерации исключения в методе start_listening_network, описание исключения сохраняется для анализа
     В деструкторе останавливает объект и поток
 */
template< typename T >
class TScopedThread
{
 public:
     /*!
       \brief Конструктор по умолчанию. Необходим для возможности создания объекта без запуска потока.
      */
     TScopedThread() = default;
     /*!
        \brief Рабочий конструктор.
        \param obj Объект, функционирующий в потоке.

        Запускает поток на выполнение. В потоке вызывает метод объекта start_listening_network.
        При генерации исключения в методе сохраняет описание исключения.
      */
     explicit TScopedThread( T *obj )
     : object_( obj )
     , t_( [this](){
         try {
             object_->start_listening_network();
         }
         catch ( const std::exception &e ) {
             *exception_.value_guard() = e.what();
         }
       })
     {}
     /*!
        \brief Запрещенный конструктор копии.
        \param orig Копируемый объект
      */
     TScopedThread( TScopedThread const &orig ) = delete;
     /*!
        \brief Запрещенный оператор присваивания.
        \param orig Копируемый объект
        \return Собственный объект
      */
     TScopedThread &operator =( TScopedThread const &rhs ) = delete;
     /*!
        \brief Деструктор класса. Вызывает метод объекта stop_listening_network. Останавливает поток выполнения
      */
     ~TScopedThread()
     {
         if( object_ )
         {
             object_->stop_listening_network();
             if( t_.joinable() )
             {
                 t_.join();
             }
         }
     }

     /*!
        \brief Возвращает описание исключения, сгенерированного в методе start_listening_network
        \return описание исключения
      */
     std::string exception()
     {
         return *exception_.value_guard();
     }

private:
     //! Объект,функционирующий в запущенном потоке
    T *object_ { nullptr };
    //! Переменная системного потока
    std::thread t_;
    //! Описание исключения, выброшенного в методе start_listening_network
    TSafeguard< std::string > exception_;
};

/*!
     \class TConfig
     \brief Класс настроек системы.

     Читает настройки либо из параметров запуска, либо из файла конфигурации, задаваемого в настройках.
 */
class TConfig
{
public:
    /*!
       \brief Конструктор по умолчанию. Необходим для возможности создания объекта без запуска потока.
      */
    TConfig() = default;
    /*!
       \brief Конструктор получения настроек
       \param argc размер списка передаваемых параметров
       \param argv список передваемых параметров
     */
    TConfig( int argc, char * argv[] );
    /*!
        \brief Запрещенный конструктор копии.
        \param orig Копируемый объект
      */
    TConfig( TConfig const &orig) = delete;
    /*!
        \brief Запрещенный оператор присваивания.
        \param orig Копируемый объект
        \return Собственный объект
      */
    TConfig &operator =( TConfig const &rhs ) = delete;

    /*!
       \class TVariant
       \brief Позволяет представлять объект вариативного типа (целочисленный, строковый, геометрии)
     */
    class TVariant
    {
    public:
        /*!
            \brief Конструктор структуры по умолчанию. Необходим для хранения объекта класса в контейнерах стандартной библиотеки.
         */
        TVariant() = default;
        /*!
           \brief Конструктор хранения целочисленной переменной
           \param v целочисленная переменная
         */
        TVariant( int v );
        /*!
           \brief Конструктор хранения строковой переменной
           \param v строковая переменная
         */
        TVariant( std::string v );
        /*!
           \brief Конструктор хранения переменной геометрии
           \param v переменная геометрии
         */
        TVariant( TGeometry const &v );
         /*!
           \brief Конструктор хранения переменной конфигурации сцен
           \param v переменная конфигурация сцен
         */
        TVariant( scene_config_t const &v );

        /*!
           \brief оператор, возвращающий целочисленную переменную хранения
         */
        operator int() const;
        /*!
           \brief оператор, возвращающий строковую переменную хранения
         */
        operator std::string() const;
        /*!
           \brief оператор, возвращающий переменную хранения геометрии
         */
        operator TGeometry() const;
        /*!
          \brief оператор, возвращающий переменную конфигурации сцен
         */
        operator scene_config_t() const;

    private:
        //! целочисленная переменная
        int ivalue_;
        //! строковая переменная
        std::string svalue_;
        //! переменная геометрии
        TGeometry gvalue_;
        //! переменная конфигурации сцен
        scene_config_t scvalue_;
    };
    //! алиас соответствия объекта TVariant ключевому строковому значению
    using fields_t = std::map< std::string, TVariant >;
    
    /*!
       \brief оператор индексации. Возвращает значение переменной по ключу
       \param key ключ поиска значения
       \return значение переменной, соответствующей ключу
     */
    TVariant &operator [](char const *key) const;
    
private:
    //! таблица конфигурации
    static fields_t fields_;

private:
    /*!
       \brief читает файл конфигурации. Заполняет таблицу конфигурации
       \param name имя файла конфигурации
     */
    void f_read_file( char const *name );
    /*!
       \brief читает файл конфигурации формата JSON. Заполняет таблицу конфигурации
       \param name имя файла конфигурации фориата JSON.
     */
    void f_read_json( char const *name );
};

/*!
     \class TImage
     \brief Структура кадра, предоставляемого абоненту.

     Задает размеры и конфиругацию кадра. Содержит его пиксельные данные и время генерации
 */
struct TImage
{
    /*!
       \brief Перечисление количества цветовых каналов в кадре
     */
    enum EColor { Monochrome = 1, RGB = 3 };

    //! Пиксельные данные кадра
    std::vector< uint8_t > pixels;
    //! Размеры кадра
    TGeometry window;
    //! Конфигурация кадра
    int channels { EColor::RGB };
    //! Время генерации кадра
    uint64_t timestamp { std::numeric_limits< uint64_t >::max() };

    /*!
        \brief Конструктор структуры по умолчанию.
     */
    TImage() = default;
    /*!
     * \brief Конструктор структуры. Настраивает переменную геометрии кадра
     * \param win
     */
    TImage( TGeometry const &win ) : window( win ) {};
};


/*!
     \class TJpegCodec
     \brief Класс декодера формата JPEG.

     Декодирует данные в формате JPEG в пиксельные данные кадра.
 */
class TJpegCodec {
public:
    /*!
        \class TError
        \brief Класс ошибки процесса декодирования

        Производный от std::runtime_error. Собственного состояния и методов не содержит
     */
    class TError: public std::runtime_error {
    public:
        /*!
            \brief Конструктор класса ошибки создания сетевого соединения
            \param what строковое описание ошибки
         */
        TError(const std::string & what);
    };

    /*!
     * \brief Конструктор класса. Настраивает структуры декодиования
     */
    TJpegCodec();
    /*!
        \brief Запрещенный конструктор копии.
        \param orig Копируемый объект
      */
    TJpegCodec(const TJpegCodec& orig) = delete;
    /*!
        \brief Запрещенный оператор присваивания.
        \param orig Копируемый объект
        \return Собственный объект
      */
    TJpegCodec &operator =(const TJpegCodec& orig) = delete;
    /*!
        \brief Деструктор класса.
     */
    ~TJpegCodec();
    
    /*!
       \brief декодирует JPEG данные из буфера. Сохраняет полученные пиксельные данные в структуру представления кадра
       \param data буфер JPEG данных
       \param size размер буфера JPEG данных
       \param img указатель на переменную хранения результата декодирования
       \return
     */
    bool decode( uint8_t const *data, size_t size, TImage *img );
    /*!
       \brief декодирует JPEG данные из файла. Сохраняет полученные пиксельные данные в структуру представления кадра
       \param filename имя файла формата JPEG
       \param img указатель на переменную хранения результата декодирования
       \return успех/неуспех процесса декодирования
     */
    bool decode( char const *filename, TImage *img );

private:
    //! контекст декодирования кадра из формата JPEG
    jpeg_decompress_struct cinfo_;
    //! контекст ошибок декодирования кадра из формата JPEG
    jpeg_error_mgr jerr_;
};

/*!
   \brief проверяет существование файла
   \param filename имя проверяемого файла
   \return результат проверки
 */
bool file_exists( char const *filename );
/*!
   \brief читает текстовый файл конфигурации. Для каждой строки вызывает функцию обработки строки
   \param fname имя файла конфигурации
   \param foo функция обработки строки
 */
void read_config( char const *fname,
                  std::function< void( const std::string& ) > foo );
/*!
   \brief разбивает строку конфигурации формата ключ=значение на соответствующие поля
   \param s строка конфигурации
   \param rc переменная пары ключ, значение
   \return удачное/неудачное разбиение
 */
bool str2key( const std::string &s, std::pair< std::string, std::string > *rc );

/*!
 * \brief читает каталог. Выбирает файлы в соответствии с фильтром. Для каждого выбранного файла выполняет функцию обработки файла
 * \param path имя каталога
 * \param filter фильтр выбора файлов в каталоге
 * \param func функция обработки файла
 */
template< typename F >
void read_directory( const std::string &path, char const *filter, F &&func )
{
    DIR *dir;
    struct dirent *entry;
    if( !(dir = opendir( path.c_str() ) ) )
        throw std::runtime_error( path + std::string(" error: ") + std::string(strerror( errno ) ) );

    while( (entry = readdir(dir))  )
    {
        if( entry->d_name[0] != '.' && entry->d_type == DT_REG && strstr( entry->d_name, filter ) )
        {
            func( entry->d_name );
        }
    }
    closedir( dir );
}

/*!
   \brief преобразует строку конфигурации формата ключ={{coord_1 ... coonrd_N} ... {coord_1 coonrd_N}} в вектор координат
 * \param s строка конфигурации
 * \param rc указатель на результирующую переменную вектора координат
 * \return удачный/неудачный результат преобразования
 */
template< typename T >
bool str2vec( const std::string &s, T *rc )
{
    std::istringstream iss(s);
    std::vector< std::string > v( (std::istream_iterator<std::string>(iss)),
                                   std::istream_iterator<std::string>() );
    if( v.size() == sizeof(T) / sizeof(float) )
    {
        for( size_t i(0); i < v.size(); ++i )
        {
            try
            {
                (*rc)[i] = std::stof(v[i]);
            }
            catch( const std::logic_error & err )
            {
                std::cerr << __PRETTY_FUNCTION__ << " error: " << err.what() <<std::endl;
                return false;
            }
        }
        return true;
    }
    return false;
}

/*!
   \brief возвращает системное время в наносекундах
   \return системное время в наносекундах
 */
inline uint64_t now()
{
    timespec ts;
    clock_gettime( CLOCK_REALTIME, &ts );

    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

}  // namespace NUtils

#endif /* UTILS_H */
