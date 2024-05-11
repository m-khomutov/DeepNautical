/*!
     \file blender.h
     \author mkh
     \date 1 марта 2023 г.
     \brief Заголовочный файл классов представления объекта, созданного в программе Blender.

     Данный файл содержит в себе состояние классов представления объекта, созданного в программе Blender , объявление их интерфейсов.
 */
#ifndef BLENDER_H
#define BLENDER_H

//#include "texture/jpeg.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace NBlender
{

/*!
   \class TMTLfile
   \brief Парсит файл материала формата .mtl.

    Выводит значения полей файла материала формата .mtl во внутренние структуры. Определяет соответствие материала
    сетке точек объекта, к которым применяется материал
 */
class TMTLfile {
public:
    //! Алиас представления плоскости объекта (матрица 3х3)
    using face_t = glm::vec< 3, glm::ivec3 >;
    /*!
       \struct TMaterial
       \brief Структура, содержащая занчения полей объекта Blender
     */
    struct TMaterial {
        //! название материала
        std::string name;

        //! коэффициент фокусировки блика (1000 - максимально сфокусированный блик)
        float Ns;
        //! коэффициент фонового освещения
        glm::vec3 Ka;
        //! коэффициент рассеянного освещения
        glm::vec3 Kd;
        //! коэффициент бликового освещения
        glm::vec3 Ks;
        //! коэффициент зеркального блика
        glm::vec3 Ke;
        //! коэффициент преломления (1.0 - преломление отсутствует)
        float Ni;
        //! коэффициент прозрачности (1.0 - полностью непрозрачен)
        float d;
        //! номер модели освещенности (0, 1, 2)
        int illum;
        //! текстура объекта
        //std::shared_ptr< TJpegTexture > map_Kd;

        //! набор точек, определяющих сетку объекта
        std::vector< face_t > faces;
    };
    //! Алиас отображения материала. Ключ - название материала
    using material_t = std::map< std::string, TMaterial >;

    /*!
       \brief Конструктор класса. Парсит файл материала
       \param filename имя файла формата .mtl
     */
    TMTLfile( char const *filename );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TMTLfile( const TMTLfile &orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TMTLfile operator =( const TMTLfile &orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TMTLfile() = default;

    /*!
       \brief operator [] возвращает материал по названию
       \param name название искомого материала
       \return объект искомого материала
     */
    TMaterial const &operator []( std::string const &name ) const;
    
private:
    //! набор материалов объекта
    material_t materials_;
};

/*!
   \class TObject
   \brief Парсит файл объекта программы blender формата .obj

    Выводит значения полей объекта программы blender формата .obj во внутренние структуры.
    Создает набор материалов объекта по файлам формата .mtl
 */
class TObject {
public:
    /*!
       \brief Конструктор класса. Парсит файл объекта
       \param filename имя файла формата .obj
     */
    explicit TObject( char const *fname );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TObject( const TObject& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TObject &operator =( const TObject& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~TObject() = default;

    /*!
       \brief заполняет вектор точек, представляющих геометрическую сетку объекта
       \param pos указатель на вектор точек, которые будут переданы в шейдер для отрисовки объекта
     */
    void load_position( std::vector< float > *pos );
    /*!
       \brief возвращает количество поверхностей сетки объекта
       \return количество поверхностей сетки объекта
     */
    size_t facecount() const
    {
        return facecount_;
    }
    /*!
       \brief возвращает ссылку на вектор материалов объекта
       \return ссылка на вектор материалов объекта
     */
    const  std::vector< TMTLfile::TMaterial > &materials() const
    {
        return materials_;
    }
    
private:
    //! набор геометрических вершин сетки объекта
    std::vector< glm::vec3 > vertices_;
    //! набор текстурных вершин сетки объекта
    std::vector< glm::vec2 > texels_;
    //! набор нормалей к геометрическим вершинам сетки объекта
    std::vector< glm::vec3 > normals_;
    //! набор объектов материалов объекта
    std::vector< TMTLfile::TMaterial > materials_;
    //! количество поверхностей сетки объекта
    size_t facecount_ { 0 };
};

}  // namespace NBlender

#endif /* BLENDER_H */
