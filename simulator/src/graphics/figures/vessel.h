/* 
 * File:   vessel.h
 * Author: mkh
 *
 * Created on 16 февраля 2023 г., 13:11
 */
/*!
     \file vessel.h
     \author mkh
     \date 16 февраля 2023 г.
     \brief Заголовочный файл класса отрисовки 3-мерной модели корабля.

     Данный файл содержит в себе состояние класса отрисовки 3-мерной модели, реализацию его интерфейсов.
 */
#ifndef VESSEL_H
#define VESSEL_H

#include "figure.h"
#include "../blender.h"

/*!
   \class TVessel
   \brief Класс отрисовки 3-мерной модели корабля
 */
class TVessel: public TFigure {
public:
    /*!
       \brief Конструктор класса. Настраивает геометрическую модель отображения
       \param settings конфигурация класса
     */
    explicit TVessel( const std::vector< std::string > &settings );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TVessel( const TVessel& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TVessel &operator =( const TVessel& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TVessel() = default;

    /*!
       \brief настраивает переменные GL и отправляет GL команду на отрисовку
       \param vbo_number номер используемого VBO (Vertex Buffer Object). Может не использоваться при наличии одного VBO
     */
    void draw();
    /*!
       \brief возвращает текущую геометрическую позиция на сцене
       \return текущая геометрическая позиция на сцене
     */
    const TFigure::TPosition &position();

private:
    //! Умный указатель на объект сетки геометрических координат фигуры
    std::unique_ptr< NBlender::TObject > object_;
    //! вектор геометрических координат, копируемых в шейдер
    std::vector< GLfloat > vertices_;
    //! 3-х мерный коэффициент ускорения движения фигуры по сцене
    QVector3D factor_;
    //! угол крена фигуры при раскачивании
    GLfloat pitching_angle_ = 0.0f;
    //! умный указатель на пустую текстуру. Используется как вспомогательная, если объект координат не представляет своей
    std::unique_ptr< QOpenGLTexture > empty_texture_;
    //! объект текуще позиции фигуры на сцене
    TFigure::TPosition position_;

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
    void f_initialize() override;
    /*!
       \brief передает свой объект посетителю, обобщающему поведение всех графических объектов на сцене
       \param vbo_number номер используемого VBO (Vertex Buffer Object)
       \param p посетитель. Объект обобщающий поведение всех графических объектов на сцене
       \param currentTime текущая временная метка
     */
    void f_accept( IVisitor &p, double currentTime ) override;

    /*!
       \brief настраивает матрицу геометрическую модели
     */
    void f_set_model();
    /*!
       \brief проверяет окончание траектории движения
       \return необходимость возврата в начальную точку (окончание траектории)
     */
    bool f_full_trajectory();
    /*!
        \brief возвращает модель в начальную точку движения
     */
    void f_reset();
};

#endif /* VESSEL_H */
