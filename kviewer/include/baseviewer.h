/* 
 * File:   baseviewer.h
 * Author: mkh
 *
 * Created on 6 февраля 2023 г., 16:09
 */
/*!
     \file baseviewer.h
     \author mkh
     \date 6 февраля 2023 г.
     \brief Заголовочный файл класса базового проектора принятого кадра.

     Данный файл содержит в себе состояние класса базового проектора принятого кадра, объявление его интерфейсов.
 */
#ifndef BASEVIEWER_H
#define BASEVIEWER_H

#include "receiver.h"
#include "basedecoder.h"
#include "utils.h"

class TBasedecoder;

/*!
   \class TBasedecoderError
   \brief Класс ошибки базового проектора принятого кадра

   Производный от std::runtime_error. Собственного состояния и методов не содержит
 */
class TBaseviewerError: public std::runtime_error
{
public:
    /*!
       \brief Конструктор класса ошибки базового проектора принятого кадра
       \param what строковое описание ошибки
     */
    TBaseviewerError( const std::string & what );
};

/*!
   \class TBaseviewer
   \brief Класс базового проектора принятого кадра

   Объявляет интерфейс старта/останова приема кадров, регистрации вывода сетевой задержки
 */
class TBaseviewer {
public:
    /*!
     * \brief Конструктор класса. Запускает поток в котором будут пиниматься видеокадры
     */
    TBaseviewer();
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TBaseviewer( const TBaseviewer& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TBaseviewer &operator =( const TBaseviewer& orig ) = delete;
    /*!
       \brief Деструктор класса. Виртуальный для обеспечения наследования
     */
    virtual ~TBaseviewer() = default;

    /*!
       \brief функция реакции на системный сигнал
     */
    virtual void onsignal( int ) = 0;

    /*!
       \brief проверяет запустился ли поток в котором должен функционировать приемник кадров. Запускает процесс, приема если поток работает
     */
    void start_stream();
    /*!
       \brief запускает процесс останова приема видеокадров
       \return
     */
    int stop_stream();
    /*!
       \brief Регистрирует обработчик вывода сетевой задержки
       \param cb регистрируемый обработчик
     */
    void register_verify_callback( TReceiver::verify_callback_t cb );

protected:
    //! переменная хранения видеокадра
    NUtils::TImage frame_;
    //! умный указатель переменной декодера видеокадра
    std::unique_ptr< TBasedecoder > decoder_;
    //! переменная сетевого приемника видеокадров
    TReceiver receiver_;
    //! переменная хранения системного потока функционирования приемника кадров
    NUtils::TScopedThread< TReceiver > rec_thread_;

private:
    /*!
       \brief объявляет метод начала приема видеокадров. Реализуется в производных классах
     */
    virtual void f_start_stream() = 0;
    /*!
       \brief объявляет метод окончания приема видеокадров. Реализуется в производных классах
     */
    virtual int f_stop_stream() = 0;
};

#endif /* BASEVIEWER_H */
