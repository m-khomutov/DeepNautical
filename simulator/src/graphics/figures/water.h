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
  \class TWater
  \brief Класс водной поверхности
 */
class TWater: public TFigure {
public:
    /*!
       \brief Конструктор класса. Настраивает геометрическую модель отображения
       \param settings конфигурация класса
       \param camera_pos позиция камеры на сцене
     */
    explicit TWater( const std::vector< std::string > &settings, const glm::vec3 &camera_pos );
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
    ~TWater() = default;

    /*!
       \brief настраивает переменные GL и отправляет GL команду на отрисовку
       \param vbo_number номер используемого VBO (Vertex Buffer Object). Может не использоваться при наличии одного VBO
     */
    void draw( size_t vbo_number );
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
    std::unique_ptr< texture > air_texture_;
    //! умный указатель на объект текстуры кильватерного следа на водной поверхности
    std::unique_ptr< texture > foam_texture_;
    //! вектор геометрических координат кильватерных следов на водной поверхности
    std::vector< TFigure::TPosition > wake_position_;

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
       \param vbo_number номер используемого VBO (Vertex Buffer Object)
     */
    void f_initialize( size_t vbo_number ) override;
    /*!
       \brief передает свой объект посетителю, обобщающему поведение всех графических объектов на сцене
       \param vbo_number номер используемого VBO (Vertex Buffer Object)
       \param p посетитель. Объект обобщающий поведение всех графических объектов на сцене
       \param currentTime текущая временная метка
     */
    void f_accept( size_t vbo_number, visitor &p, double currentTime ) override;

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

