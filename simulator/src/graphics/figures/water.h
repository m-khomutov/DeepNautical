/*!
     \file water.h
     \author mkh
     \date 1 февраля 2023 г.
     \brief Заголовочный файл класса водной поверхности.

     Данный файл содержит в себе состояние класса водной поверхности, реализацию его интерфейсов.
 */
#ifndef WATER_H
#define WATER_H

#include "figure.h"

/*!
   \class Perlin
   \brief Класс реализации шума Перлина
 */
class Perlin
{
public:
    Perlin();

    double noise( double x, double y, double z );

private:
    int p[512];

private:
    double f_fade( double t );
    double f_lerp( double t, double a, double b );
    double f_grad( int hash, double x, double y, double z );
};

/*!
  \class TWater
  \brief Класс водной поверхности
 */
class TWater: public TFigure {
public:
    /*!
       \brief Конструктор класса. Настраивает геометрическую модель отображения
       \param settings конфигурация класса
     */
    explicit TWater( const std::vector< std::string > &settings );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TWater( TWater const &orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TWater &operator =( TWater const &orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TWater();

    /*!
       \brief настраивает переменные GL и отправляет GL команду на отрисовку
     */
    void draw();
    /*!
       \brief устанавливает позицию кольветерного следа на сцене
       \param pos устанавливаемая позиция
     */
    void set_wake_position( const std::vector< TFigure::TPosition > &pos );
    
private:
    //! шаг геометрической сетки поверхности
    static const uint32_t resolution = 64;
    //! массив геометрических координат сетки поверхности
    GLfloat surface_[8 * resolution * (resolution + 1)];
    //! массив нормалей геометрических координат сетки поверхности
    GLfloat normals_[6 * resolution * (resolution + 1)];
    //! фаза отрисовки волны на поверхности
    float phase_ { 0.0f };
    //! умный указатель на объект текстуры отражения неба на водной поверхности
    std::unique_ptr< QOpenGLTexture > air_texture_;
    //! умный указатель на объект текстуры кильватерного следа на водной поверхности
    std::unique_ptr< QOpenGLTexture > foam_texture_;
    //! вектор геометрических координат кильватерных следов на водной поверхности
    std::vector< TFigure::TPosition > wake_position_;
    //! объект генерации шума Перлина
    Perlin perlin_;

private:
    /*!
       \brief проверяет валидность конфигурирования
     */
    void f_check_environment() const override;
    /*!
       \brief возвращает имя шейдера объекта
       \return имя шейдера объекта
     */
    char const *f_shader_name() const override;
    /*!
       \brief в порядке инициализации создает текстуру, выделяет память под данные и атрибуты
     */
    void f_initialize() override;
    /*!
       \brief передает свой объект посетителю, обобщающему поведение всех графических объектов на сцене
       \param p посетитель. Объект обобщающий поведение всех графических объектов на сцене
       \param currentTime текущая временная метка
     */
    void f_accept( IVisitor &p, double currentTime ) override;

    /*!
       \brief загружает геметрические координаты сетки водной поверхности в шейдер
       \param currentTime текущее время. В этом классе не используется
     */
    void f_load_surface( double currentTime );
    /*!
       \brief генерирует изменение значения ординаты точки сетки водной поверхности
       \param x абсцисса точки сетки водной поверхности
       \param z аппликата точки сетки водной поверхности
       \param in_wake указатель на переменную, определяющую, попадает ли точка сетки в кильватерный след
       \return ордината точки сетки водной поверхности
     */
    GLfloat f_generate_surface( GLfloat x, GLfloat z, GLfloat *in_wake = nullptr );
};

#endif /* WATER_H */

