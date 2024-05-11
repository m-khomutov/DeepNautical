/*!
     \file figureset.h
     \author mkh
     \date 24 января 2023 г.
     \brief Заголовочный файл класса контейнера фигур.

     Данный файл содержит в себе состояние класса контейнера фигур, объявление их интерфейсов.
 */
#ifndef FIGURESET_H
#define FIGURESET_H

#include "figures/figure.h"
#include "visitor.h"

#include <QGLWidget>

#include <vector>

/*!
   \class TFigureset
   \brief Класс контейнера геометрических фигур, отображаемых на сцене

   В классе реализована двойная диспетчеризация для возможности восстановить тип фигуры. Это нужно для создания кильватерного следа
   за объектом класса TVessel
 */
class TFigureset: public IVisitor {
public:
    //! алиас набора фигур в объекте класса
    using figure_t =  std::vector< std::shared_ptr< TFigure > >;

    /*!
     * \brief Кoнструктор класса
     */
    TFigureset() = default;
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TFigureset( const TFigureset& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TFigureset &operator =( const TFigureset &rhs ) = delete;
    /*!
       \brief Деструктор класса. Освобождает ресурсы, выделенные под используемые буферы взаимодействия с шейдерами
     */
    ~TFigureset();
    
    /*!
       \brief добавляет новую фигуру в набор фигур
       \param fig указатель на объект фигуры, помещаемой в набор
     */
    void emplace( TFigure * );

    /*!
       \brief удаляет все фигуры из набора
     */
    void clear();

    /*!
       \brief выделяет ресурсы буферам взаимодействия с шейдерам. нициализирует объекты фигур в наборе фигур
     */
    void initialize();
    /*!
       \brief инициализирует процесс отрисовки фигур сцены.
       \param currentTime текущее время

       Последовательно для каждой фигуры в наборе привязывает соответствующие буферы взаимодействия в шейдером и
       вызывает метод принятия посетителя (себя).
     */
    void draw( double currentTime );
    
    /*!
       \brief реализация метода посещения элемента класса TAntisubmarinefrigate
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    //void visit( size_t vbo_number, TAntisubmarinefrigate *fig ) override;
    /*!
       \brief реализация метода посещения элемента класса TSol
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    void visit( size_t vbo_number, TSol *fig ) override;
    /*!
       \brief реализация метода посещения элемента класса TWater
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    void visit( size_t vbo_number, TWater *fig ) override;
    /*!
       \brief реализация метода посещения элемента класса THorizon
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    //void visit( size_t vbo_number, THorizon *fig ) override;
    /*!
       \brief реализация метода посещения элемента класса TVessel
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    //void visit( size_t vbo_number, TVessel *fig ) override;
    /*!
       \brief реализация метода посещения элемента класса TSparklets
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    //void visit( size_t vbo_number, TSparklets *fig ) override;

private:
    //! буфер загрузки данных в шейдер.
    std::vector< GLuint > vbo_;
    //! буфер передачи атрибутов в шейдер
    std::vector< GLuint > vao_;
    //! буфер передачи в шейдер индеков последовательности отрисовки вершин
    std::vector< GLuint > ebo_;
    //! общее количество используемых буферов загрузки данных в шейдер
    size_t vbo_count_ { 0 };

    //! набор фигур в объекте класса
    figure_t figures_;
    //! набор геометрических позиций объектов Vessel в объекте класса (для отрисовки кильватерного следа при необходимости)
    std::vector< TFigure::TPosition > vessel_positions_;
};

#endif /* FIGURESET_H */
