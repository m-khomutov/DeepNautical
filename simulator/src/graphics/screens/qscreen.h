/*!
     \file qscreen.h
     \author mkh
     \date 5 февраля 2023 г.
     \brief Заголовочный файл Qt класса экрана отображения сцен.

     Данный файл содержит в себе состояние Qt класса экрана отображения сцен, объявление его интерфейсов.
 */
#ifndef QSCREEN_H
#define QSCREEN_H

#include "glscreen.h"
#include <QWidget>
#include <memory>

/*!
   \class TQscreen

   Создает окно и настраивает его размер
 */
class TQscreen: public TGLscreen, public QWidget {
public:
    /*!
       \brief Qt класс экрана отображения сцен.
     */
    TQscreen();
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TQscreen( const TQscreen& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TQscreen &operator =( const TQscreen& orig ) = delete;
    /*!
       \brief Деструктор Qt класса экрана отображения сцен.
     */
    ~TQscreen() = default;

    /*!
       \brief обработчик события срабатывания таймера
       \param e событие срабатывания таймера
     */
    void timerEvent( QTimerEvent *e );

protected:
    /*!
     * \brief closeEvent обработчик события закрытия окна
     * \param event событие закрытия окна
     */
    void closeEvent( QCloseEvent *event ) override;

private:
    //! идентификатор запущенного таймера
    int timer_id_ { -1 };

private:
    /*!
       \brief Реализация функции запуска отображения сцен.

       Выводит окно экрана на дистлей. Запускает таймер обновления картинки
     */
    void f_run_scene_display() override;
    /*!
       \brief Реализация функции остановки отображения сцен.

       Останавливает таймер обновления картинки
     */
    void f_stop_scene_display() override;
};

#endif /* QSCREEN_H */
