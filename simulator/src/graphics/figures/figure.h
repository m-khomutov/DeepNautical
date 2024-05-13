/*!
     \file figure.h
     \author mkh
     \date 31 января 2023 г.
     \brief Заголовочный файл класса обобщенного представления геометрической фигуры.

     Данный файл содержит в себе состояние класса обобщенного представления геометрической фигуры, реализацию его интерфейсов.
 */
#ifndef FIGURE_H
#define FIGURE_H

#include "../visitor.h"
#include "specification.h"
#include "utils.h"

#include <QGLShaderProgram>
#include <QOpenGLTexture>

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
        QVector3D course;
        //! геометрия
        QVector3D current;
        //! след (кильватерный)
        QVector2D wake;
    };

    /*!
       \brief Конструктор класса. Настраивает матрицу обзора и матрицу перспективы
       \param setting конфигурация объекта
     */
    explicit TFigure( const std::vector< std::string > &setting );
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
    virtual ~TFigure();

    /*!
       \brief создает объект GL программы. Запускает процесс инициализации, специфичный для производного класса геометрической фигуры
     */
    void initialize();
    /*!
       \brief управляет взаимодействием с посетителем
       \param p посетитель. Объект, обобщающий поведение всех графических объектов на сцене
       \param currentTime текущая временная метка

       Устанавливает используемую GL программу,
       настраивает униформные переменные и передает выполнение производному классу, для передачи указателя на себя посетителю
     */
    void accept( IVisitor &p, double currentTime );

    /*!
       \brief подтверждает выполнение настроек программы
       \return подтверждение
     */
    bool valid() const
    {
        return valid_;
    }

protected:
    //! спецификация геометрического объекта (набор параметров конфигурации)
    TSpecification spec_;
    //! GL программа
    QGLShaderProgram shader_program_;
    //! умный указатель на текстуру объекта
    std::unique_ptr< QOpenGLTexture > texture_;
    //! координаты объекта в его системе координат
    QMatrix4x4 model_;
    //! координаты для перевода объекта из его системы координат в систему координат камеры
    QMatrix4x4 view_;
    //! координаты для перевода объекта в проекционную систему координат
    QMatrix4x4 projection_;
    //! угол движения объекта относительно оси абсцисс
    QVector3D angle_ { 0.0f, 0.0f, 0.0f };
    //! позиция объекта на сцене
    float scene_position_ { 0.0f };
    //! направление движения объекта
    float direction_ { -1.0f };
    //! смещение координат объекта от центра сцены
    QVector4D offset_ { 0.f, 0.f, 0.f, 1.f };

    //! флаг готовности объекта к функционированию
    bool valid_ { false };

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
    virtual void f_initialize() = 0;
    /*!
       \brief объявляет метод передчи своего объекта посетителю, обобщающему поведение всех графических объектов на сцене
       \param vbo_number номер используемого VBO (Vertex Buffer Object)
       \param p посетитель. Объект обобщающий поведение всех графических объектов на сцене
       \param currentTime текущая временная метка
     */
    virtual void f_accept( IVisitor &p, double currentTime ) = 0;
};

#endif /* FIGURE_H */
