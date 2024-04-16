/* 
 * File:   basedecoder.h
 * Author: mkh
 *
 * Created on 26 января 2023 г., 16:40
 */

/*!
     \file basedecoder.h
     \author mkh
     \date 26 января 2023 г.
     \brief Заголовочный файл класса базового декодера принятого кадра.

     Данный файл содержит в себе состояние класса базового декодера принятого кадра, объявление его интерфейсов.
 */
#ifndef BASEDECODER_H
#define BASEDECODER_H

#include "utils.h"

#include <cstdint>
#include <vector>
#include <stdexcept>

/*!
   \class TBasedecoderError
   \brief Класс ошибки базового декодера принятого кадра

   Производный от std::runtime_error. Собственного состояния и методов не содержит
 */
class TBasedecoderError: public std::runtime_error
{
public:
    /*!
       \brief Конструктор класса ошибки базового декодера принятого кадра
       \param what строковое описание ошибки
     */
    TBasedecoderError( const std::string & what );
};

/*!
   \class TBasedecoderNodata
   \brief Класс отсутствия данных в базовом декодере

   Производный от std::runtime_error. Собственного состояния и методов не содержит
 */
class TBasedecoderNodata: public std::runtime_error
{
public:
    /*!
       \brief Конструктор класса отсутствия данных базового декодера принятого кадра
       \param what строковое описание ошибки
     */
    TBasedecoderNodata();
};

/*!
   \class TBasedecoder
   \brief Класс базового декодера принятого кадра

   Объявляет интерфейс приема, декодирования и хранения видеокадра
 */
class TBasedecoder {
public:
    /*!
       \brief Конструктор класса по умолчанию.
     */
    TBasedecoder() = default;
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TBasedecoder( const TBasedecoder& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TBasedecoder &operator =( const TBasedecoder& orig ) = delete;
    /*!
       \brief Деструктор класса. Виртуальный для обеспечения наследования
     */
    virtual ~TBasedecoder() = default;

    /*!
       \brief возвращает хранимый видеокадр. Вызывается функционал, реализованный в производных классах
       \param img указатель на переменную, в которую копируется хранимый видеокадр
       \return успех/неуспех копирования
     */
    bool copy_frame( NUtils::TImage *img );
    /*!
       \brief декодирует и сохраняет видеокадр. Вызывается функционал реализованный в производных классах
       \param frame данные принятого видеокадра
       \param size размер данных принятого видеокадра
       \param timestamp временная метка принятого видеокадра
     */
    void save_frame( uint8_t const *frame, size_t size, uint64_t timestamp );
    
protected:
    //! временная метка последнего ринятого видеокадра
    uint64_t timestamp_ { 0ul };
    
private:
    /*!
       \brief объявление метода возвращения хранимого видеокадра. Реализуется в производных классах
       \param img указатель на переменную, в которую копируется хранимый видеокадр
     */
    virtual void f_copy_frame( NUtils::TImage *img ) = 0;
    /*!
       \brief объявление метода декодирования и сохранения видеокадра. Реализуется в производных классах
       \param frame данные принятого видеокадра
       \param size размер данных принятого видеокадра
       \param timestamp временная метка принятого видеокадра
     */
    virtual void f_save_frame( uint8_t const *frame, size_t size, uint64_t timestamp ) = 0;

};

#endif /* BASEDECODER_H */
