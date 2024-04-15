/*!
     \file jpegframe.h
     \author mkh
     \date 24 января 2023 г.
     \brief Заголовочный файл класса представления видеокадра в формате JPEG.

     Данный файл содержит в себе состояние класса представления видеокадра в формате JPEG, объявление его интерфейсов.
 */

#ifndef JPEGFRAME_H
#define JPEGFRAME_H

#include "baseframe.h"

extern "C"
{
 #include <jpeglib.h>
 #include <jerror.h>
}

#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <vector>

/*!
     \class TJpegframe
     \brief Класс представления видеокадра в формате JPEG. Производный от класса TBaseframe.

      Состояние класса хранит:
      - контекст сжатия кадра в формат JPEG;
      - контекст ошибок сжатия кадра в формат JPEG;
      - буферы хранения байтов кадра. Количество буферов соответствует количеству задействованных точек обзора (сцен и камер в них).

      Реальный публичный интерфейс содержит метод предоставления указателя на буфер хранения пиксельной информации кадра для заполнения.

      Виртуальный защищенный интерфейс содержит переопределенный метод выгрузки буфера хранения байтов кадра в соответствии с протоколом передачи видео.
 */
class TJpegframe: public TBaseframe {
public:
    /*!
        \brief Конструктор класса представления видеокадра в формате JPEG.

        Создает контекст сжатия кадра в формат JPEG.
        \param geometry Размеры кадра (ширина, выстота)
        \param quality Качество сжатия JPEG (0 - 100)
        \param duration Длительность кадра (миллисекунды)
        \param reverse флаг поворота кадра на 180 градусов (по умолчанию поднят)
     */
    TJpegframe( const NUtils::TGeometry &geometry, int quality, int duration, bool reverse = true );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TJpegframe( const TJpegframe& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return
     */
    TJpegframe &operator =( const TJpegframe &rhs ) = delete;
    /*!
       \brief Деструктор базового класса представления видеокадра.
     */
    ~TJpegframe();

    /*!
        \brief Возвращает указатель на буфер хранения пиксельной информации кадра.

        Реализация функции предоставления доступа к буферу пикселей видеокадра. Объявлена в классе TBaseframe.
        При необходимости  выделяет память, определяемую размерами кадра для точки обзора.
        \param view Номер точки обзора (сцена или камера в сцене)
        \param width Ожидаемая ширина кадра
        \param height Ожидаемая высота кадра
        \return указатель на буфер хранения пиксельной информации кадра
     */
    uint8_t *buffer( size_t view, int width, int height ) override;
    /*!
        \brief Выполнить действия, подготовительные к передаче кадра абоненту - сжать кадр в формат JPEG.
        \param view Номер точки обзора (сцена или камера в сцене)
     */
    void prepare_buffer( size_t view ) override;

private:
    /*!
     * \class The TImageBuffer
     * \brief Структура, содержащая буфер для хранения данных JPEG кадра, его размеры  и размеры хранимых данных кадра
     */
    struct TImageBuffer
    {
        TBaseframe::image frame;    /// буфер данных JPEG кадра
        NUtils::TGeometry geometry; /// размеры JPEG кадра
        size_t size_ { 0 };         /// длина данных JPEG кадра
    };

    //! контекст сжатия кадра в формат JPEG
    jpeg_compress_struct cinfo_;
    //! контекст ошибок сжатия кадра в формат JPEG
    jpeg_error_mgr jerr_;

    //! буферы хранения байтов кадра
    std::vector< TImageBuffer > jpeg_frames_;
    //! флаг поворота кадра на 180 градусов
    bool reverse_ { false };

private:
    /*!
       \brief Реализация функции выгрузки буфера видеокадра абонентам.

       Использует специфический протокол передачи видео для выгрузки буфера видеокадра в формате JPEG абонентам.
       \param proto Протокол передачи видео, испольуемый для выгрузки буфера
       \return Результат выгрузки буфера
     */
    bool f_send_buffer( TBaseprotocol * proto ) override;
    /*!
       \brief Сжимает буфер пикселей кадра, заполняя JPEG буфер для отправки абоненту
       \param view Точка обзора, буфер которой сжимается
     */
    void f_compress( size_t view );
};

#endif /* JPEGFRAME_H */
