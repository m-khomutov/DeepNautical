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
#include <QOpenGLWidget>
#include <memory>

/*!
   \class TQscreen

   Создает окно и настраивает его размер
 */
class TQscreen: public TGLscreen, public QOpenGLWidget {
public:
    /*!
       \brief Qt класс экрана отображения сцен.
       \param frame указатель на объект представления видеокадра
     */
    TQscreen( TBaseframe *frame );
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
    /*!
       \brief инициализирует контекст GL. Создает текущие сцены экрана
     */
    void initializeGL() override;
    /*!
       \brief отрисовывает контекст GL

       При наличии команд экрану, выполняет их. запускает отрисовку сцен. При необходимости сохраняет текущий кадр
     */
    void paintGL() override;
    /*!
       \brief Настраивает GL вьюпорт при изменении размеров окна
       \param w новая ширина окна
       \param h новая высота окна
     */
    void resizeGL(int w, int h) override;

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
