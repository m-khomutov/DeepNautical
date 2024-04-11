/* 
 * File:   qscreen.h
 * Author: mkh
 *
 * Created on 5 февраля 2023 г., 14:43
 */

#ifndef QSCREEN_H
#define QSCREEN_H

#include "openglscreen.h"
#include <QOpenGLWidget>
#include <memory>

class qscreen: public openglscreen, public QOpenGLWidget {
public:
    qscreen( TBaseframe *frame );
    qscreen(const qscreen& orig) = delete;
    qscreen &operator =(const qscreen& orig) = delete;
    ~qscreen();

    void timerEvent(QTimerEvent*);
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    static void error_cb( int error, const GLchar * description );

private:
    int update_id_ { -1 };

private:
    void f_run() override;
    void f_stop() override;
};

#endif /* QSCREEN_H */
