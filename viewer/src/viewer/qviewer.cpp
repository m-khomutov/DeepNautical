/* 
 * File:   qviewer.cpp
 * Author: mkh
 * 
 * Created on 6 февраля 2023 г., 16:31
 */

#include "qviewer.h"
#include <QApplication>
#include <QPainter>

qviewer::qviewer()
: QWidget(nullptr)
{
    utils::geometry window = utils::config()["window"];
    setWindowTitle("ПО управления");
    setMaximumSize(window.width, window.height);
    setMinimumSize(window.width, window.height);
    img_ = QImage(size(), QImage::Format_RGB888);
}

qviewer::~qviewer()
{
}

void qviewer::onsignal( int )
{
    f_stop();
    QWidget::close();
}

void qviewer::f_run()
{
    show();
    update_tag_ = startTimer(40);
    result_ = qApp->exec();
}

int qviewer::f_stop()
{
    if(update_tag_ != -1)
    {
        killTimer(update_tag_);
    }
    update_tag_ = -1;
    return result_;
}

void qviewer::paintEvent(QPaintEvent *)
{
    if( decoder_->load( &frame_ ) )
    {
        img_ = QImage(frame_.pixels.data(),
                      frame_.window.width,
                      frame_.window.height,
                      QImage::Format_RGB888);
    }
    QPainter painter(this);
    painter.drawImage(rect(), img_, img_.rect());
}

void qviewer::timerEvent(QTimerEvent *)
{
    update();
}
