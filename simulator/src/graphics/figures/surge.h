/*!
     \file sparklets.h
     \author mkh
     \date 21 фвгуста 2023 г.
     \brief Заголовочный файл класса солнечных бликов.

     Данный файл содержит в себе состояние класса солнечных бликов, реализацию его интерфейсов.
 */
#ifndef SURGE_H
#define SURGE_H

#include "figure.h"

/*!
   \class Surge
   \brief Класс водной поверхности с бликами
 */
class TSurge: public TFigure {
public:
    /*!
       \brief Конструктор класса. Настраивает геометрическую модель отображения
       \param environment набор общих конфигурационных параметров сцены
       \param settings набор конфигурационных параметров фигуры
     */
    explicit TSurge( const NJson::TObject &environment,const NJson::TObject &settings );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TSurge( const TSurge& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TSurge &operator =( const TSurge& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TSurge() = default;

    /*!
       \brief настраивает переменные GL и отправляет GL команду на отрисовку
     */
    void draw();

private:
    //! Набор точек, представляющих блики
    std::vector< GLfloat > points_;
    //! Набор цветов, соответствующих бликам
    std::vector< GLfloat > colors_;

    //! координаты вершин
    QVector<QVector4D> layout_;
    //! текстурные координаты
    QVector<QVector2D> texels_;
    //! временная метка последней отрисовки
    GLfloat last_frame_time_ { 0.0f };
    //! умный указатель на объект текстуры воздушной среды
    std::unique_ptr< QOpenGLTexture > air_texture_;
    
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
      \brief иницализирует подложку для бликовой поверхности.

       Создает объект текстуры воздушной среды, выделяет память под атрибуты в шейдере, настраивает арибуты
     */
    void f_initialize_layout();

    /*!
       \brief Настраивает униформные переменные, отдает команду на отрисовку подложки
     */
    void f_draw_layout();
    /*!
       \brief Настраивает униформные переменные, отдает команду на отрисовку точек, представляющих блики
     */
    void f_draw_sparklets();
    
    /*!
       \brief Случайным образом устанавливает геометрическое положение точек-бликов и их цвета
     */
    void f_set_points();
    /*!
       \brief Случайным образом устанавливает геометрическое положение точек-бликов и их цвета, передает их в шейдер
     */
    void f_reset_points();
};

#endif /* SURGE_H */

