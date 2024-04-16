/*!
     \file qviewer.h
     \author mkh
     \date 6 февраля 2023 г.
     \brief Заголовочный файл класса Qt проектора принятого потока видеокадров.

     Данный файл содержит в себе состояние класса Qt проектора принятого потока видеокадров, объявление его интерфейсов.
 */
#ifndef QVIEWER_H
#define QVIEWER_H

#include "baseviewer.h"

#include <QWidget>
#include <QImage>
#include <QDebug>

/*!
   \class QSpinner
   \brief Реализует функционал отрисовки ожидания на проекторе
 */
class QSpinner {
public:
    //! количество линий по умолчанию
    static const int DEFLINES = 10;
    //! фактор скорости выцветания по умолчанию
    static const int DEFSPEED_FACTOR = 3;
    /*!
     * \brief Конструктор класса отрисовки ожидания на проекторе
     * \param rect Координаты прямоугольника отрисовки на проекторе
     * \param lines Количество линий, используемых в отрисовке (радиусы окружности символа ожидания)
     * \param speed_factor Скорость вращения по окружности ожидания
     */
    QSpinner( const QRect &rect, int lines = DEFLINES, int speed_factor = DEFSPEED_FACTOR );

    /*!
     * \brief Осуществляет отрисовку ожидания в момент времени
     * \param painter объект рисования
     */
    void paint( QPainter *painter );

private:
    //! координаты прямоугольника отрисовки на проекторе
    QRect window_;
    //! координаты центра окружности отрисовки
    QPointF center_;
    //! количество линий, используемых в отрисовке
    int lines_;
    //! угол поворота от линии к линии
    qreal angle_unit_;
    //! процент обесцвечения линии
    qreal fade_unit_;
    //! текущий угол поворота относительно горизонтали
    qreal fading_angle_ { 0.0 };
    //! максимальная скорость обесцвечения
    int speed_factor_;
    //! текущая скорость обесцвечения
    int speed_ { 0 };
};

class QViewer: public TBaseviewer, public QWidget {
public:
    /*!
       \brief Конструктор класса. Инициализирует окно просмотра
       \param update_ms период обновления окна просмотра
       \param frame_wait_ms период ожидания приема. При превышении включается отрисовка ожидания
     */
    QViewer( int update_ms, int frame_wait_ms );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    QViewer( const QViewer& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    QViewer &operator =( const QViewer& orig ) = delete;
    /*!
       \brief Деструктор класса
     */
    ~QViewer() = default;

    /*!
       \brief функция реакции на системный сигнал
     */
    void onsignal( int ) override;

private:
    /*!
       \brief реализует запуск процесса останова приема видеокадров
       \return
     */
    void f_start_stream() override;
    /*!
       \brief реализует запуск процесса останова приема видеокадров
       \return
     */
    int f_stop_stream() override;

    /*!
       \brief обрабатывает событие отрисовки окна просмотра
       \param событие отрисовки окна просмотра
     */
    void paintEvent(QPaintEvent *e);
    /*!
       \brief обрабатывает событие сработки таймера
       \param событие сработки таймера
     */
    void timerEvent(QTimerEvent*);

private:
    //! период обновления окна просмотра
    int update_period_ms_;
    //! количество циклов обновления ожидания приема, при превышении включается отрисовка ожидания
    int frame_wait_threshold_;
    //! переменная буфера изображения
    QImage img_;
    //! умный указатель объекта отрисовки ожидания соелдинения
    std::unique_ptr< QSpinner > spinner_;
    //! переменная, содержащая значение, возвращаемое системе при остановке приложения
    int result_ { EXIT_FAILURE };
    //! счетчик ожидания соединения. ПРи превышении им порога, запускается процсс отрисовки ожидания
    int noimage_counter_ { 0 };
    //! флаг удачного запуска таймера
    int update_tag_ { -1 };
};

#endif /* QVIEWER_H */
