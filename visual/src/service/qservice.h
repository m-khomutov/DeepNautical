/*!
     \file qservice.h
     \author mkh
     \date 5 февраля 2023 г.
     \brief Заголовочный файл класса сервиса отображения сцен, видеозахвата и обработки запросов абонентов.

     Данный файл содержит в себе состояние класса сервиса отображения сцен, видеозахвата и обработки запросов абонентов, объявление его интерфейсов.
 */
#ifndef QSERVICE_H
#define QSERVICE_H

#include "service.h"

/*!
   \class TQService
   \brief Класс сервиса отображения сцен, видеозахвата и обработки запросов абонентов
 */
class TQService: public base::service {
public:
    /*!
     * \brief Конструктор класса. Инициализирует формат Qt
     * \param screen объект экрана отображения сцен
     * \param port сетевой порт ожидания запросов от абонентов
     */
    TQService( base::screen *screen, uint16_t port );

    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TQService( const TQService& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TQService operator =( const TQService& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TQService() = default;

    /*!
       \brief реализация функции реакции на системный сигнал
     */
    void onsignal( int ) override;

private:
    /*!
       \brief реализация функции запуска экрана отображения сцен
     */
    void f_start_screen() override;
    /*!
       \brief реализация функции остановки экрана отображения сцен
       \return результат выполнения программы, передаваемый в систему
     */
    int f_stop_screen() override;
};

#endif /* QSERVICE_H */

