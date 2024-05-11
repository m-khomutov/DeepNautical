/*!
     \file scene.h
     \author mkh
     \date 24 января 2023 г.
     \brief Заголовочный файл класса сцены отрисовки объектов.

     Данный файл содержит в себе объявление класса сцены отрисовки объектов.
 */
#ifndef SCENE_H
#define SCENE_H

#include "figureset.h"

#include <QOpenGLDebugMessage>

#include <string>

/*!
   \class TSceneError
   \brief Класс ошибки класса сцены отрисовки объектов

   Производный от std::runtime_error. Собственного состояния и методов не содержит
 */
class  TSceneError: public std::runtime_error {
public:
    TSceneError(const std::string & what);
};

/*!
   \class TScene
   \brief Класс сцены отрисовки объектов
 */
class TScene: public QGLWidget {
public:
    /*!
       \brief Конструктор класса. Производит общую инициализацию
       \param name название сцены (по имени файла хранения сцены)
       \param specification спецификация сцены (содержит набор объектов, отрисовываемых на сцене)
       \param size размеры окна сцены
       \param position позиция сцены на экране
       \param parent указатель на объект-владелец создаваемого объекта

       Производит общую инициализацию. Настраивает GL переменные, вывод отладочных сообщений.
       Создает набор объектов для отрисовки в соответствии с переданной спецификацией
     */
    explicit TScene( const std::string &name, const std::string &specification, QSize size, QPoint position, QWidget *parent = 0 );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TScene( const TScene& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TScene & operator =( const TScene & rhs ) = delete;
    /*!
       \brief Деструктор класса сцены.
     */
    ~TScene();

    /*!
       \brief инициализирует контекст GL. Настраивает переменные GL, выставляет фоновый цвет, глубину. Выводит информацию о системе
     */
    void initializeGL() override;

    /*!
       \brief последовательно отрисовывает фигуры сцены
     */

    void paintGL() override;
    /*!
       \brief Настраивает GL вьюпорт и матрицу перспективы при изменении размеров окна
       \param w новая ширина окна
       \param h новая высота окна
     */
    void resizeGL(int w, int h) override;

    /*!
       \brief удаляет все фигуры со сцены
     */
    void clear();

    /*!
       \brief sizeHint возвращает размер окна
       \return размер окна
     */
    QSize sizeHint() const override;

    /*!
     * \brief position возвращает позицию сцены на экране
     * \return позиция сцены на экране
     */
    const QPoint& position() const;

    /*!
       \brief возвращает название сцены
       \return название сцены
     */
    const std::string name() const
    {
        return name_;
    }

public slots:
    /*!
       \brief onMessageLogged выводит отладочную информацию GL
       \param message сообщение с отдалочной информацией
     */
    void onMessageLogged(QOpenGLDebugMessage message);

private:
    //! название сцены
    std::string name_;
    //! размер окна
    QSize size_;
    //! позиция сцены на экране
    QPoint position_;
    //! Версия GL
    GLint GL_major_, GL_minor_;
    //! контейнер геометрических фигур, отображаемых на сцен
    TFigureset figureset_;
    
private:
   /*!
       \brief читает спецификацию и создает объекты, представленные в ней
       \param specification текст спецификации
     */
    void f_initialize( const std::string &specification );
    /*!
       \brief выводит отладочную информацию о GL системе
     */
    void f_debug_info();
    /*!
       \brief f_initialize_debugging инициализирует вывод сообщений от GL в режиме реального времени
     */
    void f_initialize_debugging();
    /*!
       \brief добавляет в контейнер фигур следующую фигуру из спецификации
       \param header заголовок блока данных, в котором пердставлена фигура. Соответствует типу фигуры
       \param settings набор конфигурационных параметров фигуры
     */
    void f_add_figure( const std::string &header, const std::vector< std::string > &settings );

    /*!
       \brief добавляет в контейнер фигур следующую фигуру из спецификации. Параметр шаблона соответствует типу фигуры
       \param settings набор конфигурационных параметров фигуры
     */
    template< typename Figure >
    void f_add_figure( const std::vector< std::string > &settings );
};

#endif /* SCENE_H */
