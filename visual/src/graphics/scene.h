/*!
     \file scene.h
     \author mkh
     \date 24 января 2023 г.
     \brief Заголовочный файл класса сцены отрисовки объектов.

     Данный файл содержит в себе объявление класса сцены отрисовки объектов.
 */
#ifndef SCENE_H
#define SCENE_H

#include "figureset.h"
#include <GL/glew.h>
#include <string>

/*!
   \class TSceneError
   \brief Класс ошибки класса сцены отрисовки объектов

   Производный от std::runtime_error. Собственного состояния и методов не содержит
 */
class  TSceneError: public std::runtime_error {
public:
    TSceneError(const std::string & what);
};

/*!
   \struct TDebugMessage
   \brief Сообщение, содержащее отладочную информацию выполнения команды GL

   Идиоматический класс передачи параметров в не жестко фиксированном порядке
 */
struct TDebugMessage {
    //! источник сообщения
    GLenum source;
    //! тип сообщения
    GLenum type;
    //! идентификатор сообщения
    GLuint id;
    //! серьезность ошибки, описываемой сообщением
    GLenum severity;
    //! текст сообщения
    std::string body;

    /*!
       \brief выставить источник сообщения
       \param src источник сообщения
       \return ссылка на собственный объект
     */
    TDebugMessage &set_source( GLenum src )
    {
        source = src;
        return *this;
    };
    /*!
       \brief выставить тип сообщения
       \param t тип сообщения
       \return ссылка на собственный объект
     */
    TDebugMessage &set_type( GLenum t )
    {
        type = t;
        return *this;
    };
    /*!
       \brief выставить идентификатор сообщения
       \param i идентификатор сообщения
       \return ссылка на собственный объект
     */
    TDebugMessage &set_id( GLenum i )
    {
         id = i;
         return *this;
    };
    /*!
       \brief выставить серьезность ошибки, описываемой сообщением
       \param s серьезность ошибки
       \return ссылка на собственный объект
     */
    TDebugMessage &set_severity( GLenum s )
    {
        severity = s;
        return *this;
    };
    /*!
       \brief выставить текст сообщения
       \param msg указатель на текстовый массив сообщения
       \param sz длина текстового массива сообщения
       \return ссылка на собственный объект
     */
    TDebugMessage &set_body( const GLchar * msg, GLsizei sz )
    {
        body = std::string( msg, sz );
        return *this;
    }
};

/*!
   \class TScene
   \brief Класс сцены отрисовки объектов
 */
class TScene {
public:
    /*!
       \brief Конструктор класса. Производит общую инициализацию
       \param name название сцены (по имени файла хранения сцены)
       \param specification спецификация сцены (содержит набор объектов, отрисовываемых на сцене)

       Производит общую инициализацию. Настраивает GL переменные, вывод отладочных сообщений.
       Создает набор объектов для отрисовки в соответствии с переданной спецификацией
     */
    explicit TScene( const std::string &name, const std::string &specification );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TScene( const TScene& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TScene & operator =( const TScene & rhs ) = delete;
    /*!
       \brief Деструктор класса сцены.
     */
    ~TScene() = default;

    /*!
       \brief возвращает количество камер на сцене
       \return количество камер
     */
    size_t cameras() const
    {
        return figureset_.size();
    }

    /*!
       \brief возвращает название сцены
       \return название сцены
     */
    const std::string name() const
    {
        return name_;
    }

    /*!
       \brief начинает последовательную отрисовку объектов
       \param view номер точки обзора, с которой отрисовываются объекты
       \param width ширина сцены
       \param height высота сцены
       \param currentTime текущее время
     */
    void display( size_t view, GLuint width, GLuint height, double currentTime );
    
private:
    //! название сцены
    std::string name_;
    //! вектор умных указателей на контейнеры геометрических фигур, отображаемых на сцен (по количеству точек обзора)
    std::vector< std::shared_ptr< TFigureset > > figureset_;
    
private:
    /*!
       \brief регистрируемая в GL функция отображения отладочных сообщений
       \param src источник сообщения
       \param type тип сообщения
       \param id идентификатор сообщения
       \param severity серьезность ошибки, описываемой сообщением
       \param sz длина текстового массива сообщения
       \param msg указатель на текстовый массив сообщения
       \param p указатель на пользовательский объект (здесь указатель на объект сцены отрисовки)
     */
    static void GLAPIENTRY debugCb( GLenum src,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    GLsizei sz,
                                    const GLchar * msg,
                                    const void * p );

    /*!
       \brief читает спецификацию и создает объекты, представленные в ней
       \param specification текст спецификации
     */
    void f_initialize( const std::string &specification );
    /*!
       \brief выводит отладочную информацию о GL системе
     */
    void f_debug_info();
    /*!
       \brief выводит информацию из отладочного сообщения GL системы
       \param msg отладочное сообщение
     */
    void f_debug_error( const TDebugMessage &msg ) const;
    /*!
       \brief добавляет в контейнер фигур следующую фигуру из спецификации
       \param header заголовок блока данных, в котором пердставлена фигура. Соответствует типу фигуры
       \param settings набор конфигурационных параметров фигуры
       \param camera_pos позиция камеры, с которой фигура наблюдается
     */
    void f_add_figure( const std::string &header, const std::vector< std::string > &settings, const glm::vec3 &camera_pos );

    /*!
       \brief добавляет в контейнер фигур следующую фигуру из спецификации. Параметр шаблона соответствует типу фигуры
       \param settings набор конфигурационных параметров фигуры
       \param camera_pos позиция камеры, с которой фигура наблюдается
     */
    template< typename Figure >
    void f_add_figure( const std::vector< std::string > &settings, const glm::vec3 &camera_pos );

    /*!
       \brief возвращает набор камер, представленных в параметрах конфигурации
       \param settings  параметры конфигурации
       \return набор координат камер
     */
    std::vector< glm::vec3 > f_get_camera_positions( const std::vector< std::string > &settings );
};

#endif /* SCENE_H */
