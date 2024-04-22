/*!
     \file specification.h
     \author mkh
     \date 1 марта 2023 г.
     \brief Заголовочный файл класса спецификации геометрической фигуры (настройки).

     Данный файл содержит в себе состояние класса спецификации геометрической фигуры, реализацию его интерфейсов.
 */
#ifndef SPECIFICATION_H
#define SPECIFICATION_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <vector>

/*!
   \class TSpecification
   \brief Класс спецификации геометрической фигуры (настройки).
 */
class TSpecification {
public:
    /*!
       \brief Конструктор по умолчанию. Для возможности задания значений по умолчанию.
     */
    TSpecification() = default;
    /*!
       \brief Конструктор класса. Настраивает геометрическую модель отображения
       \param settings конфигурация класса
       \param camera_pos позиция камеры на сцене
     */
    explicit TSpecification( const std::vector< std::string > &settings, const glm::vec3 &camera_pos );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TSpecification( const TSpecification& orig ) = default;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TSpecification &operator =( const TSpecification& orig ) = default;
    /*!
       \brief Деструктор класса
     */
    ~TSpecification() = default;

    //! путь к файлу шейдера отрисовки фигуры
    std::string shader_name;
    //! путь к файлу изображения текстуры фигуры
    std::string texture_name;
    //! путь к файлу изображения текстуры подложки фигуры
    std::string texture_air;
    //! путь к файлу изображения текстуры кильватерной струи фигуры
    std::string texture_foam;
    //! путь к файлу геометрических координат сетки фигуры
    std::string obj_name;
    //! путь к файлу изображения текстуры альфа-канала(прозрачности) подложки
    std::string alpha;

    //! координаты скорости перемещения фигуры по сцене
    glm::vec3 speed { 0.001f, 0.0f, 0.0f };
    //! начальные координаты фигуры на сцене
    glm::vec3 start_position = glm::vec3( 0.0f, 0.0f, 0.0f );
    //! координаты направления движения фигуры по сцене
    glm::vec3 course = glm::vec3( 0.0f, 0.0f, 0.0f );
    //! коэффициент траектории движения фигуры по сцене
    float trajectory = 1.0f;
    //! координаты раскачивания фигуры
    glm::vec2 pitching_range{ 0.0f, 0.0f };
    //! коэффициент раскачивания фигуры
    float pitching = 0.0f;
    //! координаты углового пририщения при движении фигуры по сцене
    glm::vec3 angle_gain { 0.0f, 0.0f, 0.0f };

    //! координаты начальных скоростей фигуры
    glm::vec3 start_factor = glm::vec3( 1.0f, 1.0f, 1.0f );
    //! коэффициент скорости движения фигуры
    float factor_gain = 0.0f;

    //! размеры сцены движения фигуры
    std::vector< float > viewport;

    //! позиция камеры на сцене
    glm::vec3 camera_position;

    //! цвет источника света на сцене
    glm::vec3 light_color = glm::vec3( 1.0f, 1.0f, 1.0f );
    //! позиция источника света на сцене
    glm::vec3 light_position = glm::vec3( 0.0f, 1.0f, -3.0f ); 

    //! модель волны - амплитуда, скорость
    glm::vec2 wave = { 10.0f, 0.2f };
    //! модель кильватерной струи - амплитуда, скорость
    glm::vec2 wake = { 20.0f, 2.0f };

    //! вектор цвета тумана на сцене
    glm::vec4 fog_color = {1.0f, 1.0f, 1.0f, 1.0f};
    //! коэффициент плотности тумана на сцене
    float fog_density = 0.0f;

    //! координаты ширины кильватерной струи
    glm::vec2 wake_width = {0.3f, 0.3f};
    //! координаты изменения кильватерной струи
    glm::vec2 surge[2] = { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
    //! скорость движения фигуры по сцене
    float step = 0.01;

private:
    /*!
       \brief читает координаты размера сцены для фигуры
       \param config строка конфигурации, представляющая координаты сцены
     */
    void f_read_viewport( const std::string& config );
    /*!
       \brief читает координаты изменения кильватерной струи
       \param config строка конфигурации, представляющая координаты
     */
    void f_read_surge( const std::string& config );
};

#endif /* SPECIFICATION_H */
