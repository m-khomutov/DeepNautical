/* 
 * File:   sol.h
 * Author: mkh
 *
 * Created on 1 февраля 2023 г., 10:12
 */
/*!
     \file air.h
     \author mkh
     \date 1 февраля 2023 г.
     \brief Заголовочный файл класса изображения неба.

     Данный файл содержит в себе состояние класса изображения неба, реализацию его интерфейсов.
 */
#ifndef AIR_H
#define AIR_H

#include "figure.h"

/*!
   \class TAir
   \brief Класс изображения неба
 */
class TAir: public TFigure {
public:
    /*!
       \brief Конструктор класса. Настраивает геометрическую модель отображения
       \param environment набор общих конфигурационных параметров сцены
       \param settings набор конфигурационных параметров фигуры
     */
    explicit TAir( const NJson::TObject &environment,const NJson::TObject &settings );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TAir( TAir const& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TAir &operator =( TAir const& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TAir();

    /*!
       \brief настраивает переменные GL и отправляет GL команду на отрисовку
     */
    void draw();

private:
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
     */
    void f_initialize() override;
    /*!
       \brief передает свой объект посетителю, обобщающему поведение всех графических объектов на сцене
       \param p посетитель. Объект обобщающий поведение всех графических объектов на сцене
       \param currentTime текущая временная метка
     */
    void f_accept( IVisitor &p, double currentTime ) override;
    /*!
       \brief определяет фигуру как неподвижную
       \return флаг неподвижности фигуры
     */
    bool f_moving() const override
    {
        return false;
    }
};

#endif /* AIR_H */
