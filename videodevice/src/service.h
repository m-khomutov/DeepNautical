/*!
     \file service.h
     \author mkh
     \date 4 апреля 2023 г.
     \brief Заголовочный файл класса сервиса чтения видеокадров с устройства видеозахвата.

     Данный файл содержит в себе состояние класса сервиса чтения видеокадров с устройства видеозахвата и выдачи их сетевым абонентам,
     реализацию его интерфейсов.
 */
#ifndef CAPTURE_SERVICE_H
#define CAPTURE_SERVICE_H


#include <kformat.h>

/*!
   \class TCaptureservice
   \brief Класс сервиса чтения видеокадров с устройства видеозахвата
 */
class TCaptureservice: public TBaseservice
{
public:
    /*!
     * \brief Конструктор класса
     * \param vdevname имя символьного файла устройства видеозахвата
     * \param port сетевой порт ожидания запросов от абонентов
     */
    TCaptureservice( char const *vdevname, uint16_t port );
    /*!
       \brief Деструктор класса
     */
    ~TCaptureservice() = default;

    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TCaptureservice( const TCaptureservice& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TCaptureservice &operator =( const TCaptureservice& orig ) = delete;

    /*!
       \brief функция реакции на системный сигнал
     */
    void onsignal( int ) override
    {}
};

#endif // CAPTURE_SERVICE_H
