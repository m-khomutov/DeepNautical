/*!
     \file sparklets.h
     \author mkh
     \date 21 фвгуста 2023 г.
     \brief Заголовочный файл класса солнечных бликов.

     Данный файл содержит в себе состояние класса солнечных бликов, реализацию его интерфейсов.
 */
#ifndef SPARKLETS_H
#define SPARKLETS_H

#include "figure.h"
#include "../blender.h"

/*!
   \class Sparklets
   \brief Класс солнечных бликов на водной поверхности
 */
class TSparklets: public TFigure {
public:
    /*!
       \brief Конструктор класса. Настраивает геометрическую модель отображения
       \param settings конфигурация класса
       \param camera_pos позиция камеры на сцене
     */
    explicit TSparklets( const std::vector< std::string > &settings, const glm::vec3 &camera_pos );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TSparklets( const TSparklets& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TSparklets &operator =( const TSparklets& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TSparklets() = default;

    /*!
     * \brief возвращает количество используемых Vertex Buffer Object (VBO)
     * \return количество используемых Vertex Buffer Object (VBO)
     */
    size_t vbo_count() const override
    {
        return 2;
    }

    /*!
     * \brief настраивает переменные GL и отправляет GL команду на отрисовку
     * \param vbo_number номер используемого VBO (Vertex Buffer Object). Может не использоваться при наличии одного VBO
     */
    void draw( size_t vbo_number );

private:
    //! Набор точек, представляющих блики
    std::vector< GLfloat > points_;
    //! Набор цветов, соответствующих бликам
    std::vector< GLfloat > colors_;

    //! индексы очередности отрисовки
    GLuint indices_[6] = { 0, 3, 1, 1, 3, 2 };
    //! временная метка последней отрисовки
    double last_frame_time_ { 0.0f };
    //! умный указатель на объект текстуры воздушной среды
    std::unique_ptr< TJpegTexture > air_texture_;
    
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
    void f_accept( size_t vbo_number, visitor &p, double currentTime ) override;

    /*!
      \brief иницализирует подложку для бликовой поверхности.

       Создает объект текстуры воздушной среды, выделяет память под атрибуты в шейдере, настраивает арибуты
     */
    void f_initialize_layout();
    /*!
       \brief инициализирует точки, представляющие блики.

       Выделяет память под атрибуты в шейдере, настраивает арибуты
     */
    void f_initialize_sparklets();

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

#endif /* SPARKLETS_H */

