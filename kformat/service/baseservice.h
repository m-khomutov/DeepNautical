/* 
 * File:   base.h
 * Author: mkh
 *
 * Created on 5 февраля 2023 г., 16:14
 */

#ifndef BASESERVICE_H
#define BASESERVICE_H

#include "s_poll.h"
#include "utils.h"

class basescreen;

class baseservice {
public:
    baseservice( basescreen *screen, uint16_t port );

    baseservice(const baseservice& orig) = delete;
    baseservice &operator =(const baseservice& orig) = delete;
    virtual ~baseservice();
    
    virtual void onsignal( int ) = 0;

    void run();
    int stop();

protected:
    int result_ { EXIT_SUCCESS };
    
private:
    basescreen * screen_;
    s_poll poll_;
    utils::scoped_thread< s_poll > poll_thread_;

    virtual void f_run() = 0;
    virtual int f_stop() = 0;
};

#endif /* BASE_H */

