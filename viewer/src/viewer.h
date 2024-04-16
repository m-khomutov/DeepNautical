/* 
 * File:   qviewer.h
 * Author: mkh
 *
 * Created on 6 февраля 2023 г., 16:31
 */

#ifndef QVIEWER_H
#define QVIEWER_H

#include "baseviewer.h"

#include <QWidget>
#include <QImage>
#include <QDebug>

class qspinner {
public:
    qspinner( const QRect &rect, int lines, int speed_factor );

    void paint( QPainter *painter );

private:
    QRect window_;
    QPointF center_;
    int lines_;
    qreal angle_unit_;
    qreal fade_unit_;
    qreal fading_angle_ { 0.0 };
    int speed_factor_;
    int speed_ { 0 };
};

class qviewer: public TBaseviewer, public QWidget {
public:
    qviewer();
    qviewer(const qviewer& orig) = delete;
    qviewer &operator =(const qviewer& orig) = delete;
    ~qviewer();

    void onsignal( int ) override;

private:
    void f_start_stream() override;
    int f_stop_stream() override;
    
    void paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent*);

private:
    QImage img_;
    std::unique_ptr< qspinner > spinner_;
    int result_ { EXIT_FAILURE };
    int noimage_counter_ { 0 };
    int update_tag_ { -1 };
};

#endif /* QVIEWER_H */

