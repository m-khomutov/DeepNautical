/* 
 * File:   sol.h
 * Author: mkh
 *
 * Created on 1 февраля 2023 г., 10:12
 */
/*!
     \file sol.h
     \author mkh
     \date 1 февраля 2023 г.
     \brief Заголовочный файл класса солнечного диска.

     Данный файл содержит в себе состояние класса солнечного диск, реализацию его интерфейсов.
 */
#ifndef SOL_H
#define SOL_H

#include "figure.h"

/*!
   \class TSol
   \brief Класс солнечного диска
 */
class TSol: public TFigure {
public:
    /*!
       \brief Конструктор класса. Настраивает геометрическую модель отображения
       \param settings конфигурация класса
     */
    explicit TSol( const std::vector< std::string > &settings );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TSol( TSol const& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TSol &operator =( TSol const& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TSol();

    /*!
     * \brief настраивает переменные GL и отправляет GL команду на отрисовку
     * \param vbo_number номер используемого VBO (Vertex Buffer Object). Может не использоваться при наличии одного VBO
     */
    void draw( size_t vbo_number );

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
    void f_initialize( size_t vbo_number ) override;
    /*!
       \brief передает свой объект посетителю, обобщающему поведение всех графических объектов на сцене
       \param vbo_number номер используемого VBO (Vertex Buffer Object)
       \param p посетитель. Объект обобщающий поведение всех графических объектов на сцене
       \param currentTime текущая временная метка
     */
    void f_accept( size_t vbo_number, IVisitor &p, double currentTime ) override;
};

#endif /* SOL_H */
