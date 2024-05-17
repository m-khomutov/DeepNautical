/* 
 * File:   sol.h
 * Author: mkh
 *
 * Created on 1 февраля 2023 г., 10:12
 */
/*!
     \file sky.h
     \author mkh
     \date 1 февраля 2023 г.
     \brief Заголовочный файл класса изображения неба.

     Данный файл содержит в себе состояние класса изображения неба, реализацию его интерфейсов.
 */
#ifndef SKY_H
#define SKY_H

#include "figure.h"

/*!
   \class TSky
   \brief Класс изображения неба
 */
class TSky: public TFigure {
public:
    /*!
       \brief Конструктор класса. Настраивает геометрическую модель отображения
       \param settings конфигурация класса
     */
    explicit TSky( const std::vector< std::string > &settings );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TSky( TSky const& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TSky &operator =( TSky const& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TSky();

    /*!
       \brief настраивает переменные GL и отправляет GL команду на отрисовку
     */
    void draw();

private:
    //! координаты вершин
    QVector<QVector4D> vertices_;
    //! текстурные координаты
    QVector<QVector2D> texels_;
    //! временная метка последней отрисовки
    GLfloat last_frame_time_ { 0.0f };

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
};

#endif /* SKY_H */
