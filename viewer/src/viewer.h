/* 
 * File:   viewer.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 17:49
 */

#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include <QImage>
#include "receiver/receiver.h"
#include "decoder/basedecoder.h"
#include "../../share/utils.h"
//#include <gtk/gtk.h>
#include <stdexcept>

class basedecoder;

class viewer_error: public std::runtime_error
{
public:
    viewer_error( const std::string & what );
};

class viewer: public QWidget {
public:
    viewer();
    viewer(const viewer& orig) = delete;
    viewer &operator =(const viewer& orig) = delete;
    ~viewer();

    void run();
    void stop();

private:
    void paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent*);
    
    decframe frame_;
    std::unique_ptr< basedecoder > decoder_;
    receiver receiver_;
    utils::scoped_thread< receiver > rec_thread_;
    int update_tag_ { -1 };

    QImage img_;
};

#endif /* VIEWER_H */
