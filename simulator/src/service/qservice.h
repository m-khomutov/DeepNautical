/* 
 * File:   qservice.h
 * Author: mkh
 *
 * Created on 5 февраля 2023 г., 16:47
 */

#ifndef QSERVICE_H
#define QSERVICE_H

#include "service/baseservice.h"

class qservice: public baseservice {
public:
    qservice( basescreen *screen, uint16_t port );

    qservice(const qservice& orig) = delete;
    qservice operator =(const qservice& orig) = delete;

    ~qservice() = default;
    
    void onsignal( int ) override;

private:
    void f_run() override;
    int f_stop() override;
};

#endif /* QSERVICE_H */

