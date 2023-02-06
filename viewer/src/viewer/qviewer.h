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

class qviewer: public baseviewer, public QWidget {
public:
    qviewer();
    qviewer(const qviewer& orig) = delete;
    qviewer &operator =(const qviewer& orig) = delete;
    ~qviewer();

    void onsignal( int ) override;

private:
    void f_run() override;
    int f_stop() override;
    
    void paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent*);

private:
    QImage img_;
    int result_ { EXIT_FAILURE };
};

#endif /* QVIEWER_H */

