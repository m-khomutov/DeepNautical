/*!
     \file figure.h
     \author mkh
     \date 31 января 2023 г.
     \brief Заголовочный файл класса обобщенного представления геометрической фигуры.

     Данный файл содержит в себе состояние класса обобщенного представления геометрической фигуры, реализацию его интерфейсов.
 */
#ifndef FIGURE_H
#define FIGURE_H

#include "specification.h"
#include "../visitor.h"
#include "../texture/texture.h"
#include "../program.h"
#include "utils.h"
#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

/*!
   \class TFigure
   \brief Класс обобщенного представления геометрической фигуры

   Состояние содержит переменные, общиедля всех геометрических фигур.
   Реализуется механизм отображения model:view:projection.
   Настраивает различные виды униформных переменных.
   Объявляет общий интерфейс геометричеких фигур, реализуемый в производных классах.
 */
class TFigure {
public:
    /*!
       \struct TPosition
       \brief Позиционные координаты
     */
    struct TPosition
    {
        //! курс - направление
        glm::vec3 course;
        //! геометрия
        glm::vec3 current;
        //! след (кильватерный)
        glm::vec2 wake;
    };

    /*!
       \brief Конструктор класса. Настраивает матрицу обзора и матрицу перспективы
       \param setting конфигурация объекта
       \param camera_pos позиция камеры на сцене
     */
    explicit TFigure( const std::vector< std::string > &setting, const glm::vec3 &camera_pos );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TFigure( const TFigure& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TFigure &operator=( const TFigure& orig ) = delete;
    /*!
       \brief Деструктор класса. Виртуальный для обеспечения наследования
     */
    virtual ~TFigure() = default;

    /*!
       \brief создает объект GL программы. Запускает процесс инициализации, специфичный для производного класса геометрической фигуры
       \param vbo_number номер используемого VBO (Vertex Buffer Object)
     */
    void initialize( size_t vbo_number );
    /*!
       \brief управляет взаимодействие с посетителем
       \param vbo_number номер используемого VBO (Vertex Buffer Object)
       \param p посетитель. Объект обобщающий поведение всех графических объектов на сцене
       \param currentTime текущая временная метка

       Устанавливает используемую GL программу,
       настраивает униформные переменные и передает выполнение производному классу, для передачи указателя на себя посетителю
     */
    void accept( size_t vbo_number, visitor &p, double currentTime );

    /*!
       \brief подтверждает выполнение настроек программы
       \return подтверждение
     */
    bool valid() const
    {
        return valid_;
    }
    /*!
     * \brief возвращает количество используемых VBO (Vertex Buffer Object). По умолчанию 1
     * \return количество используемых VBO
     */
    virtual size_t vbo_count() const
    {
        return 1;
    }

protected:
    //! спецификация геометрического объекта (набор параметров конфигурации)
    specification spec_;
    //! умный указатель на GL программу
    std::unique_ptr< program > program_;
    //! умный указатель на текстуру объекта
    std::unique_ptr< texture > texture_;
    //! координаты объекта в его системе координат
    glm::mat4 model_ { glm::rotate( glm::mat4(1.0f), glm::radians( 0.0f ), glm::vec3(1.0f, 0.0f, 0.0f) ) };
    //! координаты для перевода объекта из его системы координат в систему координат камеры
    glm::mat4 view_ { glm::lookAt( glm::vec3(0.0f, 0.0f, 3.0f),
                                   glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f) ) };
    //! координаты для перевода объекта в проекционную систему координат
    glm::mat4 projection_;
    //! угол движения объекта относительно оси абсцисс
    glm::vec3 angle_ = glm::vec3( 0.0f, 0.0f, 0.0f );
    //! позиция объекта на сцене
    float scene_position_ { 0.0f };
    //! направление движения объекта
    float direction_ { -1.0f };
    //! смещение координат объекта от центра сцены
    glm::vec3 offset_ { 0.0f, 0.0f, 0.0f };

    //! флаг готовности объекта к функционированию
    bool valid_ { false };

protected:
    /*!
       \brief настроить униформную переменную типа float
       \param name имя переменной в шейдере
       \param value значение, присваиваемое переменной
     */
    void set_uniform( const GLchar * name, float value );
    /*!
       \brief настроить униформную целочисленную переменную
       \param name имя переменной в шейдере
       \param value значение, присваиваемое переменной
     */
    void set_uniform( const GLchar * name, GLuint value );
    /*!
       \brief настроить униформную переменную типа массива 2-х float
       \param name имя переменной в шейдере
       \param value значение, присваиваемое переменной
     */
    void set_uniform( const GLchar * name, glm::vec2 );
    /*!
       \brief настроить униформную переменную типа массива массивов 2-х float
       \param name имя переменной в шейдере
       \param count размер настраиваемого массива
       \param value значение, присваиваемое переменной
     */
    void set_uniform( const GLchar *name, GLsizei count, glm::vec2 *value );
    /*!
       \brief настроить униформную переменную типа массива 3-х float
       \param name имя переменной в шейдере
       \param value значение, присваиваемое переменной
     */
    void set_uniform( const GLchar * name, glm::vec3 );
    /*!
       \brief настроить униформную переменную типа массива 4-х float
       \param name имя переменной в шейдере
       \param value значение, присваиваемое переменной
     */
    void set_uniform( const GLchar * name, glm::vec4 );
    /*!
       \brief настроить униформную переменную типа матрицы 3х3 float
       \param name имя переменной в шейдере
       \param value значение, присваиваемое переменной
     */
    void set_uniform( const GLchar * name, glm::mat3 );
    /*!
       \brief настроить униформную переменную типа матрицы 4х4 float
       \param name имя переменной в шейдере
       \param value значение, присваиваемое переменной
     */
    void set_uniform( const GLchar * name, glm::mat4 );
    /*!
       \brief выбрать функцию выполнения в шейдера
       \param uniform_name имя униформной переменной, задающей тип функции
       \param subroutine_name имя функции выполнения в шейдере
       \param shader_type тип шейдера (вершинный|фрагментный)
     */
    void set_subroutine( const GLchar * uniform_name, const GLchar * subroutine_name, GLenum shader_type );
    /*!
       \brief скопировать значения атрибута в шейдер
       \param name имя атрибута
       \param size размер атрибута
       \param step шаг чередования атрибутов в массиве передачи
       \param off смещение атрибута в массиве передачи
     */
    void set_attribute( char const *name, GLuint size, GLuint step, GLuint off );

private:
    /*!
       \brief объявляет метод проверки валидности конфигурирования
     */
    virtual void f_check_environment() const = 0;
    /*!
       \brief объявляет метод возврата имя шейдера объекта
       \return имя шейдера объекта
     */
    virtual char const *f_shader_name() const = 0;
    /*!
       \brief объявляет метод выделения памяти под данные и атрибуты
       \param vbo_number номер используемого VBO (Vertex Buffer Object)
     */
    virtual void f_initialize( size_t vbo_number ) = 0;
    /*!
       \brief объявляет метод передчи своего объекта посетителю, обобщающему поведение всех графических объектов на сцене
       \param vbo_number номер используемого VBO (Vertex Buffer Object)
       \param p посетитель. Объект обобщающий поведение всех графических объектов на сцене
       \param currentTime текущая временная метка
     */
    virtual void f_accept( size_t vbo_number, visitor &p, double currentTime ) = 0;
};

#endif /* FIGURE_H */
