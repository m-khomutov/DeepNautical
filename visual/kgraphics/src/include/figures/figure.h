/*!
     \file figure.h
     \author mkh
     \date 31 января 2023 г.
     \brief Заголовочный файл класса обобщенного представления геометрической фигуры.

     Данный файл содержит в себе состояние класса обобщенного представления геометрической фигуры, реализацию его интерфейсов.
 */
#ifndef FIGURE_H
#define FIGURE_H

#include "camera.h"
#include "visitor.h"
#include "specification.h"
#include "utils.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QGLShaderProgram>
#include <QOpenGLTexture>

#include <memory>

struct CU
{
    double bearing_deg;
    double elevation_deg;
    double radius;

    CU() = default;
    CU( double x, double y, double z )
    : bearing_deg( ::atan2( z, x ) * TCamera::DEG - 90. )
    , elevation_deg( ::atan2(::sqrt( x * x + z * z), y ) * TCamera::DEG - 90. )
    , radius( ::sqrt( x * x + y * y + z * z ) )
    {}
    CU( const QVector4D& cartesian )
    : CU( cartesian.x(), cartesian.y(), cartesian.z() )
    {}

    CU& normalize()
    {
        bearing_deg = TCamera::clip(bearing_deg);
        elevation_deg = TCamera::clip(elevation_deg);
        return *this;
    }

    QVector3D toCartesian() const
    {
        return QVector3D( radius * ::sinf( TCamera::RAD * elevation_deg ) * ::cosf( TCamera::RAD * bearing_deg ),
                          radius * ::sinf( TCamera::RAD * elevation_deg ) * ::sinf( TCamera::RAD * bearing_deg ),
                          radius * ::cosf( TCamera::RAD * elevation_deg ) );
    }

    bool neighbour( const CU& other, double dist ) const
    {
        return (toCartesian() - other.toCartesian()).length() < dist;
    }

    double distance( float x, float y, float z ) const
    {
        return (toCartesian() - QVector3D(x, y, z)).length();
    }

};

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
       \brief Конструктор класса. Настраивает геометрическую модель отображения
       \param environment набор общих конфигурационных параметров сцены
       \param settings набор конфигурационных параметров фигуры
     */
    explicit TFigure( const NJson::TObject &environment,const NJson::TObject &settings );

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
    void accept( IVisitor &p, double currentTime, const TCamera &camera );

    /*!
       \brief coordinates возвращает текущие координаты объекта на сцене
       \return текущие координаты объекта на сцене
     */
    const QVector4D &coordinates() const
    {
        return offset_;
    }
    /*!
       \brief подтверждает выполнение настроек программы
       \return подтверждение
     */
    bool valid() const
    {
        return valid_;
    }
    /*!
       \brief подтверждает наличие движения у валидной фигуры
       \return флаг движения валидной фигуры
     */
    bool moving() const;

    /*!
       \brief distance посчитать расстояние до точки
       \param point точка целеположения
       \return расстояние
     */
    double distance( const CU &point );

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
    QMatrix4x4 camera_view_;
    //! угол движения объекта относительно оси абсцисс
    QVector3D angle_ { 0.0f, 0.0f, 0.0f };
    //! позиция объекта на сцене
    float scene_position_ { 0.0f };
    //! направление движения объекта
    float direction_ { -1.0f };
    //! смещение координат объекта от центра сцены
    QVector4D offset_ { 0.f, 0.f, 0.f, 1.f };

    QOpenGLVertexArrayObject vertex_array_object_;
    QOpenGLBuffer vertex_buffer_object_;

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
    /*!
       \brief определяет фигуру как движущуюся или нет
       \return флаг движения фигуры
     */
    virtual bool f_moving() const = 0;

};

#endif /* FIGURE_H */
