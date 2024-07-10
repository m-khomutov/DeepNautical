/*!
     \file jpeg.h
     \author mkh
     \date 30 января 2023 г.
     \brief Заголовочный файл класса текстуры JPEG формата.

     Данный файл содержит в себе состояние класса текстуры JPEG формата, объявление его интерфейсов.
 */
#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"
#include <GL/glew.h>
#include <stdexcept>

/*!
   \class TJpegTextureError
   \brief Класс ошибки текстуры JPEG формата

   Производный от std::runtime_error. Собственного состояния и методов не содержит
 */
class TJpegTextureError: public std::runtime_error {
public:
    /*!
       \brief Конструктор класса ошибки текстуры JPEG формата
       \param what строковое описание ошибки
     */
    TJpegTextureError(const std::string & what);
};

/*!
   \class TJpegTexture

   Загружает из файла формата JPEG кадр, используемый как текстурное изображение
 */
class TJpegTexture {
public:
    /*!
       \brief Конструктор класса. Читает файл формата JPEG. Настраивает параметры текстуры
       \param filename имя файла формата JPEG
       \param alpha имя файла формата JPEG, изображение которого используется как alpha-канал (прозрачность)
     */
    explicit TJpegTexture( char const *filename, char const *alpha = nullptr );
    /*!
       \brief Конструктор класса. Создает текстуру из буфера
       \param img объект кадра, содержащий буфер изображения для текстуры
     */
    explicit TJpegTexture( NUtils::TImage &img );
    /*!
       \brief Конструктор класса. Создает пустую текстуру
       \param w ширина текстуры
       \param h высота текстуры
       \param value значение, присваиваемое пикселам текстуры
     */
    TJpegTexture( GLuint w, GLuint h, uint8_t value );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TJpegTexture( const TJpegTexture& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TJpegTexture &operator =( const TJpegTexture& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TJpegTexture();

    /*!
       \brief Конструктор копии из буфера.
       \param img Копируемый объект
     */
    TJpegTexture &operator =( NUtils::TImage &img );
    
    /*!
       \brief активиркет текстуру
       \param number номер активируемой текстуры
     */
    void activate( uint16_t number = GL_TEXTURE0 ) const;

private:
    //! идентификатор, присвоенный текстуре
    GLuint id_;
};

#endif /* TEXTURE_H */
