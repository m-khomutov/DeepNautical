/* 
 * File:   viewer.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 17:49
 */

#include <QPainter>

#include "viewer.h"
#include "decoder/jpegdecoder.h"
#include "../../share/utils.h"
#include <iostream>


viewer_error::viewer_error( const std::string &what )
: std::runtime_error( what )
{
}

viewer::viewer()
: QWidget(nullptr)
, frame_(  utils::config()["window"] )
, decoder_( new jpegdecoder )
, receiver_( decoder_.get() )
, rec_thread_( &receiver_ )
{
    utils::geometry window = utils::config()["window"];
    setWindowTitle("ПО управления");
    setMaximumSize(window.width, window.height);
    setMinimumSize(window.width, window.height);
    img_ = QImage(size(), QImage::Format_RGB888);
}

viewer::~viewer()
{
}

void viewer::run()
{
    show();
    update_tag_ = startTimer(40);
}

void viewer::stop()
{
    if(update_tag_ != -1)
        killTimer(update_tag_);
    update_tag_ = -1;
}


void viewer::paintEvent(QPaintEvent *)
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

void viewer::timerEvent(QTimerEvent *)
{
    update();
}
