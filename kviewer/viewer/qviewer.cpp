/* 
 * File:   qviewer.cpp
 * Author: mkh
 * 
 * Created on 6 февраля 2023 г., 16:31
 */

#include "qviewer.h"
#include <QApplication>
#include <QPainter>

// спиннер ожидания (крутящаяся палочка)
QSpinner::QSpinner( const QRect &rect, int lines, int speed_factor_ )
: window_( rect )
, center_( rect.width() / 2, rect.height() / 2 )
, lines_( lines )
, angle_unit_( 360.0 / lines )
, fade_unit_ ( 1.0 / lines )
, speed_factor_( speed_factor_ )
{}

void QSpinner::paint( QPainter *painter )
{
    painter->fillRect( window_, {100, 100, 100, 128} );
    painter->setPen( {200, 200, 255} );
    painter->setFont( {"arial, helvetica", 48} );
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


QViewer::QViewer( int update_ms, int frame_wait_ms )
: QWidget(nullptr)
, update_period_ms_( update_ms )
, frame_wait_threshold_( frame_wait_ms / update_ms )
{
    NUtils::TGeometry window = NUtils::TConfig()["window"];
    setWindowTitle("Ocean 3D");
    setMaximumSize(window.width, window.height);
    setMinimumSize(window.width, window.height);
    img_ = QImage(size(), QImage::Format_RGB888);
    spinner_.reset( new QSpinner( rect() ) );
}

void QViewer::onsignal( int )
{
    f_stop_stream();

    QWidget::close();
}

void QViewer::f_start_stream()
{
    show();
    // таймер обновления
    update_tag_ = startTimer( update_period_ms_ );
    result_ = qApp->exec();
}

int QViewer::f_stop_stream()
{
    if(update_tag_ != -1)
    {
        killTimer(update_tag_);
    }
    update_tag_ = -1;

    return result_;
}

void QViewer::paintEvent(QPaintEvent *)
{
    // увеличить счетчик ожидания изображения. если изображение есть, обнулится.
    if( noimage_counter_ < frame_wait_threshold_ )
        ++noimage_counter_;

    QPainter painter(this);
    try
    {
        if( decoder_->copy_frame( &frame_ ) ) // есть изображения
        {
            img_ = QImage(frame_.pixels.data(),
                          frame_.window.width,
                          frame_.window.height,
                          QImage::Format_RGB888);
            noimage_counter_ = 0;
        }
        else if( noimage_counter_ >= frame_wait_threshold_ ) // нет изображения
        {
            // превышен порог ожидания изображения. Надо спиннер крутить
            spinner_->paint( &painter );
            return;
        }
        painter.drawImage(rect(), img_, img_.rect()); // рисуем кадр
    }
    catch( const std::exception &e )
    {
        qDebug() << "copy frame error: " << e.what();  // посмотреть ошибку
        if( noimage_counter_ >= frame_wait_threshold_ )
        {
            // превышен порог ожидания изображения. Надо спиннер крутить
            spinner_->paint( &painter );
        }
    }
}

void QViewer::timerEvent(QTimerEvent *)
{
    update();
}
