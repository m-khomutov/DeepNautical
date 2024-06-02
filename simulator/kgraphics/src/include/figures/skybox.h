#ifndef SKYBOX_H
#define SKYBOX_H

#include "figure.h"

#include <string>

#include <QVector>

class TSkybox: public TFigure {
public:
    /*!
       \brief Конструктор класса. Настраивает геометрическую модель отображения
       \param environment набор общих конфигурационных параметров сцены
       \param settings набор конфигурационных параметров фигуры
     */
    explicit TSkybox( const NJson::TObject &environment,const NJson::TObject &settings );
    TSkybox( TSkybox const &orig ) = delete;
    TSkybox &operator =( TSkybox const &orig ) = delete;
    ~TSkybox() = default;

     void draw();

private:
    GLfloat position_[3 * 6 * 6] = {    // текстурные координаты
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        +1.0f, -1.0f, -1.0f,
        +1.0f, -1.0f, -1.0f,
        +1.0f, +1.0f, -1.0f,
        -1.0f, +1.0f, -1.0f,

        -1.0f, -1.0f, +1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, +1.0f, -1.0f,
        -1.0f, +1.0f, -1.0f,
        -1.0f, +1.0f, +1.0f,
        -1.0f, -1.0f, +1.0f,

        +1.0f, -1.0f, -1.0f,
        +1.0f, -1.0f, +1.0f,
        +1.0f, +1.0f, +1.0f,
        +1.0f, +1.0f, +1.0f,
        +1.0f, +1.0f, -1.0f,
        +1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, +1.0f,
        -1.0f, +1.0f, +1.0f,
        +1.0f, +1.0f, +1.0f,
        +1.0f, +1.0f, +1.0f,
        +1.0f, -1.0f, +1.0f,
        -1.0f, -1.0f, +1.0f,

        -1.0f, +1.0f, -1.0f,
        +1.0f, +1.0f, -1.0f,
        +1.0f, +1.0f, +1.0f,
        +1.0f, +1.0f, +1.0f,
        -1.0f, +1.0f, +1.0f,
        -1.0f, +1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, +1.0f,
        +1.0f, -1.0f, -1.0f,
        +1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, +1.0f,
        +1.0f, -1.0f, +1.0f};

    GLuint indices_[6*6] = { 0, 1, 3, 1, 2, 3,
                            7, 5, 4, 7, 6, 5,
                           11, 9, 8, 11, 10, 9,
                           15, 13, 12, 15, 14, 13,
                           19, 17, 16, 19, 18, 17,
                           23, 21, 20, 23, 22, 21
                           };
//    GLfloat position_[20] = {    // текстурные координаты
//            20.0f, 0.01f, 20.f,    1.0f, 0.0f,
//            20.0f, 0.01f, -20.0f,     1.0f, 1.0f,
//           -20.0f, 0.01f, -20.0f,     0.0f, 1.0f,
//           -20.0f, 0.01f, 20.f,    0.0f, 0.0f,
//    };
//    GLuint indices_[6] = { 0, 1, 3, 1, 2, 3 };

    std::unique_ptr< QOpenGLTexture > skybox_;

private:
    void f_check_environment() const override;
    char const *f_shader_name() const override;
    void f_initialize() override;
    void f_accept( IVisitor &p, double currentTime ) override;
    /*!
       \brief определяет фигуру как неподвижную
       \return флаг неподвижности фигуры
     */
    bool f_moving() const override
    {
        return false;
    }

    void f_create_face( TSpecification::ECubeFaces cube_face, GLenum face );
};



#endif // SKYBOX_H
