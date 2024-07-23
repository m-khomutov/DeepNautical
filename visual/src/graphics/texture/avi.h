/*!
     \file avi.h
     \author mkh
     \date 12 февраля 2023 г.
     \brief Заголовочный файл класса текстуры AVI формата.

     Данный файл содержит в себе состояние класса текстуры AVI формата, объявление его интерфейсов.
 */
#ifndef AVI_H
#define AVI_H

#include "utils.h"
#include <fstream>
#include <stdexcept>
#include <memory>
#include <vector>

/*!
   \class TAviTextureError
   \brief Класс ошибки текстуры AVI формата

   Производный от std::runtime_error. Собственного состояния и методов не содержит
 */
class TAviTextureError: public std::runtime_error {
public:
    /*!
       \brief Конструктор класса ошибки текстуры AVI формата
       \param what строковое описание ошибки
     */
    TAviTextureError(const std::string & what);
};

/*!
   \class TAviTexture

   Загружает из файла формата AVI кадры, используемые как текстурное изображение
 */
class TAviTexture {
public:
    /*!
       \struct TFrame
       \brief Структура кадра из формата AVI
     */
    struct TFrame {
    public:
        /*!
           \brief Конструктор структуры по умолчанию
         */
        TFrame() = default;
        /*!
           \brief Конструктор структуры, собирающий значение полей зи файла формата AVI
           \param f дескриптор файла формата AVI
         */
        explicit TFrame( FILE *f );

        /*!
           \brief определяет валидность считанного кадра по первому jpeg-тегу (ffd8)
           \param f дескриптор файла формата AVI
           \return
         */
        bool valid( FILE *f ) const;

        //! положение начала кадра в файле формата AVI
        size_t begin { 0 };
        //! размер кадра в файле формата AVI
        size_t size { 0 };
    };
    //! алиас набора кадров
    using frame_t = std::vector< TFrame >;
    
    /*!
       \class TAtom
       \brief Класс тега файла формата AVI
     */
    class TAtom {
    public:
        //! Перечисление тегов, используемых форматом AVI
        enum EType: uint32_t {
            riff = 0x46464952,
            lst  = 0x5453494c,
            avih = 0x68697661,
            strh = 0x68727473,
            strf = 0x66727473,
            movi = 0x69766f6d,
        };

        /*!
           \brief читает тег из файла формата AVI
           \param f дескриптор файла формата AVI
           \param frames указатель на набор кадров, содержащихся в теге
           \return умный указатель на созданный объект тега
         */
        static std::unique_ptr< TAtom > make( FILE *f, frame_t *frames );

        /*!
           \brief Конструктор класса. Настраивает 4-х байтный тип тега
           \param fourcc тип тега
         */
        explicit TAtom( uint32_t fourcc )
        : fourcc_( fourcc )
        {}
        /*!
           \brief Запрещенный конструктор копии.
           \param orig Копируемый объект
         */
        TAtom( const TAtom& orig ) = delete;
        /*!
           \brief Запрещенный оператор присваивания.
           \param orig Копируемый объект
           \return Собственный объект
         */
        TAtom &operator =( const TAtom& orig) = delete;
        /*!
           \brief Виртуальный деструктор класса.
         */
        virtual ~TAtom() = default;

        /*!
           \brief возвращает 4-х байтный тип тега
           \return 4-х байтный тип тега
         */
        uint32_t fourcc() const
        {
            return fourcc_;
        };
        /*!
           \brief возвращает размер тега
           \return размер тега
         */
        uint32_t size() const
        {
            return size_;
        };
        /*!
           \brief приводит тег к строковому виду
           \return строковый вид тега
         */
        std::string to_str() const;

    protected:
        //! 4-х байтный тип тега
        uint32_t fourcc_;
        //! размер тега
        uint32_t size_;
    };
    
    /*!
       \class Tavih
       \brief Основной заголовок AVI файла.

       Определяется блоком с FOURCC 'avih'. Cодержит глобальную информацию для всего файла
     */
    class Tavih: public TAtom {
    public:
        /*!
           \brief Конструктор класса. Читает поля из файла AVI формата
           \param f дескриптор файла формата AVI
           \param frames указатель на объект набора кадров. Передается для выделения памяти под frame_count кадров
         */
        Tavih( FILE *f, frame_t *frames );

        //! определяет количество микросекунд между кадрами
        uint32_t framerate_mcs;
        //! указывает примерную максимальную скорость передачи данных файла
        uint32_t transferrate;
        //! резерв
        uint32_t padding;
        //! содержит перечень свойств видеофайла
        uint32_t flags;
        //! указывает общее количество кадров в файле
        uint32_t frame_count;
        //! определяет начальные кадры
        uint32_t initial_frames;
        //! указывает количество потоков в файле
        uint32_t streams;
        //! определяет рекомендуемый размер буфера для чтения файла
        uint32_t suggested_bufsize;
        //! задает ширину кадра
        uint32_t width;
        //! задает высоту кадра
        uint32_t height;
    };
    
    /*!
       \class Tstrh
       \brief Класс заголовка потока
     */
    class Tstrh: public TAtom {
    public:
        /*!
           \brief Конструктор класса. Читает поля из файла AVI формата
           \param f дескриптор файла формата AVI
         */
        explicit Tstrh( FILE *f );

        //! определяет тип данных в потоке. Здесь актуален 'vids'
        uint32_t fcc_type;
        //! определяет тип оптимальный тип обработчика данных потока
        uint32_t fcc_handler;
        //! лаги, применяемые к потоку
        uint32_t flags;
        //! приоритет потока при нескольких в файле
        uint16_t priority;
        //! язык представления потока
        uint16_t language;
        //! разрыв между аудио сэмплом и соответствующим ему видео кадром
        uint32_t initial_frames;
        //! временная шкала выдачи кадров
        uint32_t scale;
        //! скорость выдачи кадров
        uint32_t rate; /* dwRate / dwScale == samples/second */
        //! начальная временная метка файла
        uint32_t start;
        //! длина потока
        uint32_t length; /* In units above... */
        //! рекомендуемый размер буфера для чтения потока
        uint32_t suggested_bufsize;
        //! индикатор качества потока
        uint32_t quality;
        //! размер кадра. Если размеры меняются, равен нулю
        uint32_t samplesize;
        //! геометрические размеры и местоположение кадра
        uint32_t frame[ 4 ];
    };
    
    /*!
       \class Tstrf
       \brief Класс блока формата потока

       описывает формат данных в потоке
     */
    class Tstrf: public TAtom {
    public:
        /*!
           \brief Конструктор класса. Читает поля из файла AVI формата
           \param f дескриптор файла формата AVI
         */
        explicit Tstrf( FILE *f );

        //! количество байтов, необходимых для структуры
        uint32_t bisize;
        //! ширина растрового изображения в пикселях
        uint32_t width;
        //! высота растрового изображения в пикселях
        uint32_t height;
        //! количество плоскостей для целевого устройства
        uint16_t planes;
        //! количество битов на пиксель (bpp).
        uint16_t bitcount;
        //! вид компрессии в формате кода FOURCC
        uint32_t compression;
        //! размер изображения (в байтах)
        uint32_t sizeimage;
        //! горизонтальное разрешение (в пикселях на метр)
        uint32_t xppm;
        //! вертикальное разрешение (в пикселях на метр)
        uint32_t yppm;
        //! количество цветовых индексов в таблице цветов, которые фактически используются растровым рисунком
        uint32_t clrused;
        //! количество цветовых индексов, которые считаются важными для отображения растрового изображения
        uint32_t clrimportant;
    };
    
    /*!
       \class Tlist
       \brief Класс блока LIST, соддержащего тег
     */
    class Tlist: public TAtom {
    public:
        /*!
         * \brief Конструктор класса. Читает блок, заполняет объект набора кадров кадрами этого блока
         * \param type Тип тег
         * \param f дескриптор файла формата AVI
         * \param frames указатель на заполняемых объект набора кадров
         */
        Tlist( TAtom::EType type, FILE *f, frame_t *frames );

    private:
        //! тип тега
        TAtom::EType type_;
    };


    /*!
     * \brief Конструктор класса. Читает файл формата AVI. Собирает теги из данных файла
     * \param filename имя файла формата AVI
     */
    explicit TAviTexture( char const *filename );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TAviTexture( const TAviTexture& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TAviTexture operator =( const TAviTexture& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TAviTexture() = default;

    /*!
     * \brief  возвращает следующий кадр из файла формата AVI
     * \return следующий кадр из файла формата AVI
     */
    utils::frame &next_image();

    /*!
     * \brief возвращает длительность кадра, считанную из файла формата AVI
     * \return длительность кадра
     */
    double frame_duration() const
    {
        return frame_duration_;
    }
    
private:
    /*!
       \class Tclfl
       \brief Вспомогательный класс автоматического закрытия файла
     */
    struct TFileCloser {
        /*!
           \brief закрывает файл переданного дескриптора файла
           \param f дескриптор закрываемого файла
         */
        void operator()( FILE *f )
        {
            fclose( f );
        }
    };
    //! умный указатель дескриптора файла формата AVI
    std::unique_ptr< FILE, TFileCloser > ifile_;

    //! объект хранения набора кадров
    frame_t frames_;
    //! объект-итератор по кадрам
    frame_t::iterator frame_iter_;
    //! буфер под кадр в формате JPEG
    std::vector< uint8_t > jpeg;
    //! декодер кадра в формате JPEG
    utils::jcodec codec_;
    //! объект кадра, представляемого как текстура
    utils::frame image_;
    //! длительность кадра
    double frame_duration_ {0.0f };
};

#endif /* AVI_H */
