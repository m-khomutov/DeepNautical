/*!
     \file specification.h
     \author mkh
     \date 1 марта 2023 г.
     \brief Заголовочный файл класса спецификации геометрической фигуры (настройки).

     Данный файл содержит в себе состояние класса спецификации геометрической фигуры, реализацию его интерфейсов.
 */
#ifndef SPECIFICATION_H
#define SPECIFICATION_H

#include "utils.h"

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
       \param environment набор общих конфигурационных параметров сцены
       \param settings набор конфигурационных параметров фигуры
     */
    explicit TSpecification( const NJson::TObject &environment,const NJson::TObject &settings );
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

    enum ECubeFaces { LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK, CUBE_FACES };

    //! путь к файлу шейдера отрисовки фигуры
    std::string shader_name;
    //! путь к файлу изображения текстуры фигуры
    std::string texture_name;
    //! путь к файлу изображения текстуры подложки фигуры
    std::string texture_air;
    //! путь к файлу изображения текстуры кильватерной струи фигуры
    std::string texture_foam;
    //! путь к файлу геометрических координат сетки фигуры
    std::string blenderobj_name;
    //! путь к файлу изображения текстуры альфа-канала(прозрачности) подложки
    std::string texture_alpha;

    //! путь к файлам изображений кубической текстуры
    std::array< std::string, CUBE_FACES > cube_faces;
    //! координаты скорости перемещения фигуры по сцене
    QVector3D speed { 0.f, 0.f, 0.f };
    //! начальные координаты фигуры на сцене
    QVector3D start_position { 0.f, 0.f, 0.f };
    //! координаты направления движения фигуры по сцене
    QVector3D course { 0.f, 0.f, 0.f };
    //! коэффициент траектории движения фигуры по сцене
    float trajectory = 1.f;
    //! координаты раскачивания фигуры
    QVector2D pitching_range { 0.f, 0.f };
    //! коэффициент раскачивания фигуры
    float pitching = 0.f;
    //! координаты углового пририщения при движении фигуры по сцене
    QVector3D angle_gain { 0.f, 0.f, 0.f };

    //! координаты начальных скоростей фигуры
    QVector3D start_factor { 1.f, 1.f, 1.f };
    //! коэффициент скорости движения фигуры
    QVector3D factor_gain { 0.f, 0.f, 0.f };

    //! размеры сцены движения фигуры
    std::vector< float > viewport;

    //! позиция камеры на сцене
    QVector3D camera_position;

    //! цвет источника света на сцене
    QVector3D light_color { 1.f, 1.f, 1.f };
    //! позиция источника света на сцене
    QVector3D light_position { 0.f, 1.f, -3.f };

    //! модель волны - амплитуда, скорость
    QVector2D wave { 10.f, 0.2f };
    //! модель кильватерной струи - амплитуда, скорость
    QVector2D wake { 20.f, 2.f };

    //! вектор цвета тумана на сцене
    QVector4D fog_color = {1.f, 1.f, 1.f, 1.f};
    //! коэффициент плотности тумана на сцене
    float fog_density = 0.f;

    //! координаты ширины кильватерной струи
    QVector2D wake_width {0.3f, 0.3f};
    //! координаты изменения кильватерной струи
    QVector2D ripple[2] = { QVector2D(0.f, 0.f), QVector2D(0.f, 0.f) };
    //! скорость движения фигуры по сцене
    float step = 0.1;
    //! путь к файлу изображения текстуры запотевания камеры
    std::string texture_sweat;
    //! флаг разрешения отрисовки запотевания камеры
    bool sweat_enabled {false};
    //! флаг разрешения отрисовки дождя
    bool rain_enabled {false};
    //! векторный коэффициент плотности дождя
    QVector4D rain_severity = {0.1f, 0.1f, 0.1f, 0.3f};
};

#endif /* SPECIFICATION_H */
