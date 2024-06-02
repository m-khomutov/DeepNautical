/*!
     \file baseservice.h
     \author mkh
     \date 5 февраля 2023 г.
     \brief Заголовочный файл базового класса сервиса отображения сцен, видеозахвата и обработки запросов абонентов.

     Данный файл содержит в себе состояние базового класса сервиса отображения сцен, видеозахвата и обработки запросов абонентов, объявление его интерфейсов.
 */
#ifndef BASESERVICE_H
#define BASESERVICE_H

#include "s_poll.h"
#include "utils.h"

class TBasescreen;

/*!
     \class TBaseservice
     \brief Базовый класс сервиса отображения сцен, видеозахвата и обработки запросов абонентов.

      Состояние класса хранит:
      - указатель на объект экрана отображения сцен;
      - объект сетевого взаимодействия с абонентами;
      - объект потока сетевого взаимодействия с абонентами;

      Реальный публичный интерфейс определяет методы:
      - запуска экрана отображения сцен;
      - остановки экрана отображения сцен;
      - запуска процесса получения кадров от платы видеозахвата;
      - остановки процесса получения кадров от платы видеозахвата;

      Виртуальный публичный интерфейс объявляет метод реакции на системный сигнал

      Виртуальный защищенный интерфейс объявляет методы:
      - запуска экрана отображения сцен;
      - остановки экрана отображения сцен;
 */
class TBaseservice {
public:
    /*!
       \brief Конструктор базового класса сервиса отображения сцен и обработки запросов абонентов.
       \param screen объект экрана отображения сцен
       \param port сетевой порт ожидания запросов от абонентов
     */
    TBaseservice( TBasescreen *screen, uint16_t port );
    /*!
       \brief Конструктор базового класса сервиса видеозахвата и обработки запросов абонентов.
       \param videodevname имя символьного файла взаимодействия с платой видеозахвата
       \param port сетевой порт ожидания запросов от абонентов
     */
    TBaseservice( char const *videodevname, uint16_t port );

    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TBaseservice( const TBaseservice& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TBaseservice &operator =( const TBaseservice& orig ) = delete;
    /*!
       \brief Виртуальный деструктор базового класса сервиса отображения сцен, видеозахвата и обработки запросов абонентов.
     */
    virtual ~TBaseservice();
    
    /*!
       \brief функция реакции на системный сигнал
     */
    virtual void onsignal( int ) = 0;

    /*!
       \brief запускает экран отображения сцен
     */
    void start_screen();
    /*!
       \brief останавливает экран отображения сцен
       \return результат выполнения программы, передаваемый в систему
     */
    int stop_screen();

    /*!
       \brief запускает процесс получения кадров от платы видеозахвата
     */
    void start_vdev_capture();
    /*!
       \brief останавливает процесс получения кадров от платы видеозахвата
     */
    void stop_vdev_capture();

protected:
    //! возвращаемое программой число при остановке программы
    int result_ { EXIT_SUCCESS };

private:
    //! указатель на объект экрана отображения сцен;
    TBasescreen * screen_ { nullptr };
    //! объект сетевого взаимодействия с абонентами;
    TSpoll poll_;
    //! объект потока сетевого взаимодействия с абонентами;
    NUtils::TScopedThread< TSpoll > poll_thread_;

private:
    /*!
       \brief Объявление функции запуска экрана отображения сцен. Может переопределяться в производных классах.
     */
    virtual void f_start_screen()
    {}
    /*!
       \brief Объявление функции остановки экрана отображения сцен. Может переопределяться в производных классах.
       \return результат выполнения программы, передаваемый в систему
     */
    virtual int f_stop_screen()
    {
        return EXIT_SUCCESS;
    }
};

#endif /* BASESERVICE_H */
