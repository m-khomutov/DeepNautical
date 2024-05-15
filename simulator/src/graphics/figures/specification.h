/*!
     \file specification.h
     \author mkh
     \date 1 марта 2023 г.
     \brief Заголовочный файл класса спецификации геометрической фигуры (настройки).

     Данный файл содержит в себе состояние класса спецификации геометрической фигуры, реализацию его интерфейсов.
 */
#ifndef SPECIFICATION_H
#define SPECIFICATION_H

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

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
    explicit TSpecification( const std::vector< std::string > &settings );
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
    //! путь к файлу изображения пустой текстуры
    std::string empty_texture;
    //! путь к файлу геометрических координат сетки фигуры
    std::string obj_name;
    //! путь к файлу изображения текстуры альфа-канала(прозрачности) подложки
    std::string alpha;

    //! координаты скорости перемещения фигуры по сцене
    QVector3D speed { 0.001f, 0.0f, 0.0f };
    //! начальные координаты фигуры на сцене
    QVector3D start_position { 0.0f, 0.0f, 0.0f };
    //! координаты направления движения фигуры по сцене
    QVector3D course { 0.0f, 0.0f, 0.0f };
    //! коэффициент траектории движения фигуры по сцене
    float trajectory = 1.0f;
    //! координаты раскачивания фигуры
    QVector2D pitching_range { 0.0f, 0.0f };
    //! коэффициент раскачивания фигуры
    float pitching = 0.0f;
    //! координаты углового пририщения при движении фигуры по сцене
    QVector3D angle_gain { 0.0f, 0.0f, 0.0f };

    //! координаты начальных скоростей фигуры
    QVector3D start_factor { 1.0f, 1.0f, 1.0f };
    //! коэффициент скорости движения фигуры
    QVector3D factor_gain { 0.f, 0.f, 0.f };

    //! размеры сцены движения фигуры
    std::vector< float > viewport;

    //! позиция камеры на сцене
    QVector3D camera_position;

    //! цвет источника света на сцене
    QVector3D light_color { 1.0f, 1.0f, 1.0f };
    //! позиция источника света на сцене
    QVector3D light_position { 0.0f, 1.0f, -3.0f };

    //! модель волны - амплитуда, скорость
    QVector2D wave { 10.0f, 0.2f };
    //! модель кильватерной струи - амплитуда, скорость
    QVector2D wake { 20.0f, 2.0f };

    //! вектор цвета тумана на сцене
    QVector4D fog_color = {1.0f, 1.0f, 1.0f, 1.0f};
    //! коэффициент плотности тумана на сцене
    float fog_density = 0.0f;

    //! координаты ширины кильватерной струи
    QVector2D wake_width {0.3f, 0.3f};
    //! координаты изменения кильватерной струи
    QVector2D surge[2] = { QVector2D(0.0f, 0.0f), QVector2D(0.0f, 0.0f) };
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
