/* 
 * File:   qservice.h
 * Author: mkh
 *
 * Created on 5 февраля 2023 г., 16:47
 */

#ifndef QSERVICE_H
#define QSERVICE_H

#include "baseservice.h"
#include <memory>

class qservice: public baseservice {
public:
    qservice();
    qservice(const qservice& orig) = delete;
    qservice operator =(const qservice& orig) = delete;
    ~qservice() = default;
    
    void onsignal( int ) override;

private:
    void f_run() override;
    int f_stop() override;
};

#endif /* QSERVICE_H */

