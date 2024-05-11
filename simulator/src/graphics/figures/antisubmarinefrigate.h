/*!
     \file antisubmarinefrigate.h
     \author mkh
     \date 31 января 2023 г.
     \brief Заголовочный файл класса 2-мерного силуэта корабля.

     Данный файл содержит в себе состояние класса 2-мерного силуэта корабля, реализацию его интерфейсов.
 */
#ifndef ANTISUBMARINEFRIGATE_H
#define ANTISUBMARINEFRIGATE_H

#include "figure.h"

/*!
   \class TAntisubmarinefrigate
   \brief Класс 2-мерного силуэта корабля
 */
class TAntisubmarinefrigate: public TFigure {
public:
    /*!
       \brief Конструктор класса. Настраивает геометрическую модель отображения
       \param settings конфигурация класса
     */
    explicit TAntisubmarinefrigate( const std::vector< std::string > &settings );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TAntisubmarinefrigate( const TAntisubmarinefrigate& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TAntisubmarinefrigate &operator=( const TAntisubmarinefrigate& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TAntisubmarinefrigate() = default;

    /*!
       \brief настраивает переменные GL и отправляет GL команду на отрисовку
       \param vbo_number номер используемого VBO (Vertex Buffer Object). Может не использоваться при наличии одного VBO
     */
    void draw( size_t vbo_number );

private:
    //! текстурные координаты
    GLfloat position_[20] = {
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f,
            0.5f, -0.5f, 0.0f,   1.0f, 1.0f,
           -0.5f, -0.5f, 0.0f,   0.0f, 1.0f,
           -0.5f,  0.5f, 0.0f,   0.0f, 0.0f,
    };
    //! индексы очередности отрисовки
    GLuint indices_[6] = { 0, 1, 3, 1, 2, 3 };
    //! позиция ватерлинии
    float waterline_ { 0.67f };

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
    /*!
       \brief настраивает геометрическую модель отображения
     */
    void f_set_model();
};

#endif /* ANTISUBMARINEFRIGATE_H */
