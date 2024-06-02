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
#include "camera.h"
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
     * \brief closest вернуть ближайшую к точке фигуру
     * \param point точка
     * \return ближайшая фигура изи nullptr
     */
    const TFigure *closest_on_distance( const CU& point, double distance );

    /*!
       \brief выделяет ресурсы буферам взаимодействия с шейдерам. нициализирует объекты фигур в наборе фигур
     */
    void initialize();
    /*!
       \brief инициализирует процесс отрисовки фигур сцены.
       \param currentTime текущее время
       \param camera текущее положение камеры на сцене

       Последовательно для каждой фигуры в наборе привязывает соответствующие буферы взаимодействия в шейдером и
       вызывает метод принятия посетителя (себя).
     */
    void draw( double currentTime, const TCamera &camera );
    
    /*!
       \brief реализация метода посещения элемента класса TAir
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    void visit( TAir *fig ) override;
    /*!
       \brief реализация метода посещения элемента класса TWater
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    void visit( TWaves *fig ) override;
    /*!
       \brief реализация метода посещения элемента класса TVessel
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    void visit( TVessel *fig ) override;
    /*!
       \brief реализация метода посещения элемента класса TSurge
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    void visit( TSurge *fig ) override;

    void visit( TSkybox *fig ) override;

private:
    //! набор фигур в объекте класса
    figure_t figures_;
    //! набор геометрических позиций объектов Vessel в объекте класса (для отрисовки кильватерного следа при необходимости)
    std::vector< TFigure::TPosition > vessel_positions_;
};

#endif /* FIGURESET_H */
