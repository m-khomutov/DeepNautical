/* 
 * File:   qservice.h
 * Author: mkh
 *
 * Created on 5 февраля 2023 г., 16:47
 */

#ifndef QSERVICE_H
#define QSERVICE_H

#include "kformat/service/baseservice.h"

class qservice: public TBaseservice {
public:
    qservice( TBasescreen *screen, uint16_t port );

    qservice(const qservice& orig) = delete;
    qservice operator =(const qservice& orig) = delete;

    ~qservice() = default;
    
    void onsignal( int ) override;

private:
    void f_start_screen() override;
    int f_stop_screen() override;
};

#endif /* QSERVICE_H */

