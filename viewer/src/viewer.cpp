/* 
 * File:   qviewer.cpp
 * Author: mkh
 * 
 * Created on 6 февраля 2023 г., 16:31
 */

#include "viewer.h"
#include <QApplication>
#include <QPainter>

qspinner::qspinner( const QRect &rect, int lines, int speed_factor_ )
: window_( rect )
, center_( rect.width() / 2, rect.height() / 2 )
, lines_( lines )
, angle_unit_( 360.0 / lines )
, fade_unit_ ( 1.0 / lines )
, speed_factor_( speed_factor_ )
{}

void qspinner::paint( QPainter *painter )
{
    painter->fillRect( window_, {100, 100, 100, 128} );
    painter->setPen( {200, 200, 255} );
    painter->setFont( {"arial,helvetica", 48} );
    painter->drawText( window_, "Соединение...", Qt::AlignCenter | Qt::AlignTop );

    painter->setPen(Qt::NoPen);
    painter->save();
    painter->translate( center_ );
    painter->rotate( fading_angle_ );
    for( int i(0); i < lines_; ++i )
    {
        QColor cl( 0, 0, 0 );
        cl.setAlphaF( fade_unit_ * i );
        painter->setBrush( cl );

        painter->rotate( angle_unit_ );
        painter->translate( 0, 20 );
        painter->drawRoundedRect( QRect(-5, 0, 10, 40), 80, 80, Qt::RelativeSize );
        painter->translate( 0, -20 );
    }
    painter->restore();
    ++ speed_;
    if( speed_ == speed_factor_ )
    {
        fading_angle_ += angle_unit_;
        if( fading_angle_ > 360.0 )
        {
	    fading_angle_ = 0.0;
        }
        speed_ = 0;
    }
}


qviewer::qviewer()
: QWidget(nullptr)
{
    NUtils::TGeometry window = NUtils::TConfig()["window"];
    setWindowTitle("ПО управления");
    setMaximumSize(window.width, window.height);
    setMinimumSize(window.width, window.height);
    img_ = QImage(size(), QImage::Format_RGB888);
    spinner_.reset( new qspinner( rect(), 10, 3 ) );
}

qviewer::~qviewer()
{}

void qviewer::onsignal( int )
{
    f_stop_stream();
    QWidget::close();
}

void qviewer::f_start_stream()
{
    show();
    update_tag_ = startTimer(40);
    result_ = qApp->exec();
}

int qviewer::f_stop_stream()
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
    if( noimage_counter_ < 50 )
        ++noimage_counter_;

    QPainter painter(this);
    try
    {
        if( decoder_->copy_frame( &frame_ ) )
        {
            img_ = QImage(frame_.pixels.data(),
                          frame_.window.width,
                          frame_.window.height,
                          QImage::Format_RGB888);
            noimage_counter_ = 0;
        }
        else if( noimage_counter_ >= 50 )
        {
            spinner_->paint( &painter );
            return;
        }
        painter.drawImage(rect(), img_, img_.rect());
    }
    catch( const std::exception &e )
    {
        qDebug() << "copy frame error: " << e.what();
        if( noimage_counter_ >= 50 )
        {
            spinner_->paint( &painter );
        }
    }
}

void qviewer::timerEvent(QTimerEvent *)
{
    update();
}
