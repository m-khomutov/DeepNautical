/*!
     \file jpegdecoder.h
     \author mkh
     \date 26 января 2023 г.
     \brief Заголовочный файл класса декодера принятого кадра формата JPEG.

     Данный файл содержит в себе состояние класса декодера принятого кадра формата JPEG, объявление его интерфейсов.
 */
#ifndef JPEGDECODER_H
#define JPEGDECODER_H

#include "basedecoder.h"

#include <mutex>

/*!
   \class TJpegdecoder
   \brief Класс декодера принятого кадра формата JPEG

   Реализует интерфейс приема, декодирования и хранения видеокадра
 */
class TJpegdecoder: public TBasedecoder {
public:
    /*!
       \brief Конструктор по умолчанию
     */
    TJpegdecoder() = default;
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TJpegdecoder( const TJpegdecoder& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TJpegdecoder &operator =( const TJpegdecoder& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TJpegdecoder() = default;

private:
    //! переменная класса кодека формата JPEG
    NUtils::TJpegCodec codec_;
    //! буферы приема кадра и хранения кадра. Кадр принимается в 0-й буфер и при декодировании копируется в 1-й
    std::vector< uint8_t > frame_[2];
    //! мьютекс копии кадра из 0-го буфера в 1-й
    std::mutex mutex_;

private:
    /*!
       \brief копирует хранимый видеокадр в предоставляемую по указателю переменную.
       \param img указатель на переменную, в которую копируется хранимый видеокадр
     */
    void f_copy_frame( NUtils::TImage *img ) override;
    /*!
       \brief декодирует и сохраняет видеокадр.
       \param frame данные принятого видеокадра
       \param size размер данных принятого видеокадра
       \param timestamp временная метка принятого видеокадра
     */
    void f_save_frame( uint8_t const *frame, size_t size, uint64_t timestamp ) override;
};

#endif /* JPEGDECODER_H */
