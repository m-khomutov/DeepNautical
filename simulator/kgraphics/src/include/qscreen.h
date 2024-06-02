/*!
     \file qscreen.h
     \author mkh
     \date 5 февраля 2023 г.
     \brief Заголовочный файл Qt класса экрана отображения сцен.

     Данный файл содержит в себе состояние Qt класса экрана отображения сцен, объявление его интерфейсов.
 */
#ifndef QSCREEN_H
#define QSCREEN_H

#include <kformat.h>

#include "scene.h"
#include "screencommand.h"


#include <QWidget>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <stdexcept>

/*!
   \class TQScreenError
   \brief Класс ошибки класса экрана отображения сцен

   Производный от std::runtime_error. Собственного состояния и методов не содержит
 */
class TQScreenError: public std::runtime_error {
public:
    /*!
       \brief Конструктор класса ошибки создания сетевого соединения
       \param what строковое описание ошибки
     */
    TQScreenError(const std::string & what);
};

/*!
   \class TQscreen

   Создает окно и настраивает его размер
 */
class TQscreen: public TBasescreen, public QWidget {
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

    /*!
       \brief Возвращает список имен сцен, связанных с экраном
       \return список сцен
     */
    std::vector< std::string > get_scenes() const override;

protected:
    //! вектор указателей на сцены, выполняемые на экране
    std::vector< TScene* > sc_;
    //! Очередь команд, передаваемых экрану для выполнения
    NUtils::TSafeguard< std::list< TScreenCommand > > commands_;

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
    /*!
       \brief Реализация функции отправки сохраненного кадра сцены абоненту по определенному сетевому протоколу.
       \param proto сетевой протокол выдачи видеопотока абоненту
       \return результат отправки (удалось/не удалось)
     */
    void f_send_stored_scene_frame( TBaseprotocol *proto ) override;
    /*!
       \brief при наличии команд в очереди команд запускает их последовательно на выполнение
     */
    void f_exec_command();
};

#endif /* QSCREEN_H */
