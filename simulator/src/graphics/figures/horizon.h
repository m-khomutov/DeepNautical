/* 
 * File:   horizon.h
 * Author: mkh
 *
 * Created on 13 февраля 2023 г., 17:58
 */
/*!
     \file horizon.h
     \author mkh
     \date 13 февраля 2023 г.
     \brief Заголовочный файл класса отображения неба и линии горизонта.

     Данный файл содержит в себе состояние класса отображения неба и линии горизонта, реализацию его интерфейсов.
 */
#ifndef HORIZON_H
#define HORIZON_H

#include "figure.h"
#include "../texture/avi.h"

/*!
   \class THorizon
   \brief Класс отображения неба и линии горизонта.
 */
class THorizon: public TFigure {
public:
    /*!
       \brief Конструктор класса. Настраивает геометрическую модель отображения
       \param settings конфигурация класса
       \param camera_pos позиция камеры на сцене
     */
    explicit THorizon( const std::vector< std::string > &settings, const glm::vec3 &camera_pos );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    THorizon( const THorizon& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    THorizon &operator =( const THorizon& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~THorizon() = default;

    /*!
     * \brief настраивает переменные GL и отправляет GL команду на отрисовку
     * \param vbo_number номер используемого VBO (Vertex Buffer Object). Может не использоваться при наличии одного VBO
     */
    void draw( size_t vbo_number );
    
private:
    //! индексы очередности отрисовки координат
    GLuint indices_[6] = { 0, 3, 1, 1, 3, 2 };
    //! умный указатель на объект кадров формата AVI
    std::unique_ptr< TAviTexture > avi_;
    //! Временноая метка последней отрисовки
    double last_frame_time_ { 0.0f };

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
    void f_accept( size_t vbo_number, visitor &p, double current_time ) override;
};

#endif /* HORIZON_H */

