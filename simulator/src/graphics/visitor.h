/*!
     \file visitor.h
     \author mkh
     \date 18 апреля 2023 г.
     \brief Заголовочный файл класса абстрактного посетителя.

     Данный файл содержит в себе объявление интерфейса абстрактного посетителя.
 */
#ifndef VISITOR_H
#define VISITOR_H

#include <cstddef>

class TSol;
class TWater;
class TSurge;
/*class TAntisubmarinefrigate;
class THorizon;
class TVessel;*/

/*!
   \interface IVisitor
   \brief Описывает общий интерфейс для всех типов посетителей.

   Объявляет набор методов, отличающихся типом входящего параметра, которые нужны для запуска операции для всех типов конкретных элементов
 */
class IVisitor {
public:
    /*!
       \brief Деструктор класса.
     */
    virtual ~IVisitor() = default;

    /*!
       \brief объявление метода посещения элемента класса TAntisubmarinefrigate
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    //virtual void visit( size_t vbo_number, TAntisubmarinefrigate * ) = 0;
    /*!
       \brief объявление метода посещения элемента класса TSol
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    virtual void visit( size_t vbo_number, TSol * ) = 0;
    /*!
       \brief объявление метода посещения элемента класса TWater
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    virtual void visit( size_t vbo_number, TWater * ) = 0;
    /*!
       \brief объявление метода посещения элемента класса THorizon
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    //virtual void visit( size_t vbo_number, THorizon * ) = 0;
    /*!
       \brief объявление метода посещения элемента класса TVessel
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    //virtual void visit( size_t vbo_number, TVessel * ) = 0;
    /*!
       \brief объявление метода посещения элемента класса TSparklets
       \param vbo_number номер связанного VBO
       \param fig посещаемый элемент
     */
    virtual void visit( size_t vbo_number, TSurge * ) = 0;
};

#endif /* VISITOR_H */
