/* 
 * File:   qservice.cpp
 * Author: mkh
 * 
 * Created on 5 февраля 2023 г., 16:47
 */

#include "qservice.h"
#include <QApplication>
#include <QSurfaceFormat>

TQService::TQService( TBasescreen *screen, uint16_t port )
: TBaseservice( screen, port )
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3,3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(format);
    QApplication::setAttribute(Qt::AA_ForceRasterWidgets, false);
}

void TQService::f_start_screen()
{
    result_ = qApp->exec();
}

int TQService::f_stop_screen()
{
    return result_;
}

void TQService::onsignal( int )
{
    qApp->quit();
}
