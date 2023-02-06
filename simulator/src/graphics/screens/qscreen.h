/* 
 * File:   qscreen.h
 * Author: mkh
 *
 * Created on 5 февраля 2023 г., 14:43
 */

#ifndef QSCREEN_H
#define QSCREEN_H

#include "basescreen.h"
#include "../scene.h"
#include <QOpenGLWidget>
#include <memory>

class qscreen: public basescreen, public QOpenGLWidget {
public:
    explicit qscreen( baseframe *frame );
    qscreen(const qscreen& orig) = delete;
    qscreen &operator =(const qscreen& orig) = delete;
    ~qscreen();

    void run() override;
    void stop() override;

    baseframe *frame_;
    std::unique_ptr<scene> sc_;
    int update_id_ { -1 };

    void timerEvent(QTimerEvent*);
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    static void error_cb( int error, const GLchar * description );
private:

};

#endif /* QSCREEN_H */
