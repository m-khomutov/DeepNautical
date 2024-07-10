/*!
     \file glscreen.h
     \author mkh
     \date 5 февраля 2023 г.
     \brief Заголовочный файл GL класса экрана отображения сцен.

     Данный файл содержит в себе состояние GL класса экрана отображения сцен, объявление его интерфейсов.
 */
#ifndef OPENGLSCREEN_H
#define OPENGLSCREEN_H

#include <kformat.h>

#include "../scene.h"
#include <list>
#include <set>
#include <string>
#include <stdexcept>

/*!
   \class TGlScreenError
   \brief Класс ошибки GL класса экрана отображения сцен

   Производный от std::runtime_error. Собственного состояния и методов не содержит
 */
class TGLscreenError: public std::runtime_error {
public:
    /*!
       \brief Конструктор класса ошибки создания сетевого соединения
       \param what строковое описание ошибки
     */
    TGLscreenError(const std::string & what);
};

/*!
   \class TScreenCommand
   \brief Класс команды, передаваемой экрану от сетевого абонента
 */
class TScreenCommand {
public:
    //! Типы передаваемых команд
    enum EScreenCommandType { kSetScene };

    /*!
       \brief Конструктор класса. Настраивает тип создаваемой команды
       \param t тип создаваемой команды
       \param pos позиция
     */
    TScreenCommand( EScreenCommandType t, size_t pos )
    : t_( t )
    , position_( pos )
    {}

    /*!
       \brief Возвращает тип команды
       \return тип команды
     */
    EScreenCommandType type() const
    {
        return t_;
    }
    /*!
     * \brief Возвращает позицию
     * \return позиция
     */
    size_t position() const
    {
        return position_;
    }

private:
    //! тип команды
    EScreenCommandType t_;
    //! позиция
    size_t position_;
};

/*!
   \class TGLscreen
   \brief GL класс экрана отображения сцен

   Создает список имен сцен, связанных с экраном. Настраивает итератор по именам сцен на начало списка сцен
 */
class TGLscreen: public TBasescreen {
public:
    /*!
       \brief GL класс экрана отображения сцен.
       \param frame указатель на объект представления видеокадра
     */
    TGLscreen( TBaseframe *frame );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TGLscreen( const TGLscreen& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TGLscreen &operator =( const TGLscreen& orig ) = delete;
    /*!
       \brief Виртуальный деструктор GL класса экрана отображения сцен.
     */
    virtual ~TGLscreen() = default;

    /*!
       \brief Возвращает список имен сцен, связанных с экраном
       \return список сцен
     */
    const std::set< std::string > &get_scenes() const override
    {
        return scenes_;
    }
    /*!
       \brief Возвращает имена текущих сцен, выполняющихся на экране
       \return текущие сцены, выполняющиеся на экране
     */
    std::vector< std::string > current_scenes() override;
    /*!
     * \brief Настраивает итератор по именам сцен на имя желаемой сцены
     * \param scene имя желаемой сцены
       \param position место позиционирования сцены
     */
    void set_scene( const std::string &scene, size_t position ) override;

protected:
    //! Список имен сцен, связанных с экраном
    std::set< std::string > scenes_;
    //! итератор по сценам, настроенный на текущую сцену
    std::set< std::string >::iterator scene_iter_;
    //! вектор умных указателей на сцены, выполняемые на экране
    std::vector< std::shared_ptr< TScene > > sc_;
    //! Очередь команд, передаваемых экрану для выполнения
    NUtils::TSafeguard< std::list< TScreenCommand > > commands_;

protected:
    /*!
     * \brief при наличии команд в очереди команд запускает их последовательно на выполнение
     */
    void f_exec_command();

private:
    /*!
      \brief Реализация функции сохранения текущего кадра сцены. Объявляется в базовом классе.
     */
    void f_store_scene_frame() override;
    /*!
       \brief Реализация функции отправки сохраненного кадра сцены абоненту по определенному сетевому протоколу.
       \param proto сетевой протокол выдачи видеопотока абоненту
       \return результат отправки (удалось/не удалось)
     */
    bool f_send_stored_scene_frame( TBaseprotocol *proto ) override;
};

#endif /* OPENGLSCREEN_H */

