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
#include "camera.h"
#include <kformat.h>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
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
class TScene: public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    /*!
       \brief Конструктор класса. Производит общую инициализацию
       \param config конфигурация сцены (содержит набор объектов, отрисовываемых на сцене)
       \param parent указатель на объект-владелец создаваемого объекта

       Производит общую инициализацию. Настраивает GL переменные, вывод отладочных сообщений.
       Создает набор объектов для отрисовки в соответствии с переданной спецификацией
     */
    explicit TScene( const NUtils::TSceneConfig &config, QWidget *parent = nullptr );
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
       \brief sizeHint возвращает размер окна
       \return размер окна
     */
    QSize minimumSizeHint() const override;

    /*!
       \brief position возвращает позицию сцены на экране
       \return позиция сцены на экране
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

    /*!
       \brief camera возвращает положение камеры на сцене
       \return положение камеры на сцене
     */
    const TCamera & camera() const
    {
        return camera_;
    }

    /*!
       \brief удаляет все фигуры со сцены
     */
    void clear();

    void zoomCamera( float factor );

    void rotateCamera( float x, float y );

    void setColorMode( int cl_mode );

    void setPanorama( int factor );

    /*!
       \brief Отправить сохраненный кадр сцены абоненту по определенному сетевому протоколу.
       \param proto сетевой протокол выдачи видеопотока абоненту
       \return результат отправки (удалось/не удалось)
     */
    bool send_frame( TBaseprotocol *proto );

signals:
    void clicked();

public slots:
    /*!
       \brief onMessageLogged выводит отладочную информацию GL
       \param message сообщение с отдалочной информацией
     */
    void onMessageLogged(QOpenGLDebugMessage message);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* e) override;
    void keyPressEvent(QKeyEvent* e);

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
    //! указатель на объект представления видеокадра;
    std::unique_ptr< TBaseframe > frame_;
    //! мьютекс обеспечения потокобезопасной работы с объектом представления видеокадра;
    std::mutex frame_mutex_;
    //! временная метка сохранения видеокадра в объекте представления видеокадра;
    TBaseframe::time_point_t store_ts_;
    //! объект камеры с проекцией
    TCamera camera_;
    //! текущие координаты мыши на сцене
    QPoint mouse_pos_;
    int panorama_ {0};
    int cl_mode_ {0};
    int capture_mode_ {0};

    QVector< CU > designationCoords_ {8};

    const TFigure *captured_target_ {nullptr};
    qint64 capture_time_ {0};

private:
   /*!
       \brief читает спецификацию и создает объекты, представленные в ней
       \param specification текст спецификации
     */
    void f_initialize( const NJson::TObject &specification );
    /*!
       \brief выводит отладочную информацию о GL системе
     */
    void f_debug_info();
    /*!
       \brief f_initialize_debugging инициализирует вывод сообщений от GL в режиме реального времени
     */
    void f_initialize_debugging();

    void f_capture_target();

    /*!
       \brief добавляет в контейнер фигур следующую фигуру из спецификации
       \param environment набор общих конфигурационных параметров сцены
       \param settings набор конфигурационных параметров фигуры
     */
    void f_add_figure( const NJson::TObject &environment, const NJson::TObject &settings );

    /*!
       \brief добавляет в контейнер фигур следующую фигуру из спецификации. Параметр шаблона соответствует типу фигуры
       \param environment набор общих конфигурационных параметров сцены
       \param settings набор конфигурационных параметров фигуры
     */
    template< typename Figure >
    void f_add_figure( const NJson::TObject &environment,const NJson::TObject &settings );
    /*!
       \brief f_store_frame сохраняет текущий фрейм изображения
     */
    void f_store_frame();
};

#endif /* SCENE_H */
