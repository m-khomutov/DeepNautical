/* 
 * File:   baseviewer.h
 * Author: mkh
 *
 * Created on 6 февраля 2023 г., 16:09
 */

#ifndef BASEVIEWER_H
#define BASEVIEWER_H

#include "../receiver/receiver.h"
#include "../decoder/basedecoder.h"
#include "utils.h"

class basedecoder;

class viewer_error: public std::runtime_error
{
public:
    viewer_error( const std::string & what );
};

class baseviewer {
public:
    static std::unique_ptr< baseviewer > make();

    baseviewer();
    baseviewer(const baseviewer& orig) = delete;
    baseviewer &operator =(const baseviewer& orig) = delete;
    virtual ~baseviewer();

    virtual void onsignal( int ) = 0;

    void run();
    int stop();

protected:
    NUtils::image frame_;
    std::unique_ptr< basedecoder > decoder_;
    receiver receiver_;
    NUtils::scoped_thread< receiver > rec_thread_;
    int update_tag_ { -1 }; 

private:
    virtual void f_run() = 0;
    virtual int f_stop() = 0;

};

#endif /* BASEVIEWER_H */
