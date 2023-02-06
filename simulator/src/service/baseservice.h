/* 
 * File:   base.h
 * Author: mkh
 *
 * Created on 5 февраля 2023 г., 16:14
 */

#ifndef BASESERVICE_H
#define BASESERVICE_H


#include "../graphics/screens/basescreen.h"
#include "../server/s_poll.h"
#include "../encoding/jpegframe.h"
#include "../../../share/utils.h"

class baseservice {
public:
    static baseservice *make();
    
    baseservice(const baseservice& orig) = delete;
    baseservice &operator =(const baseservice& orig) = delete;
    virtual ~baseservice();
    
    virtual void onsignal( int ) = 0;

    void run();
    int stop();

protected:
    std::unique_ptr< baseframe > frame_;
    std::unique_ptr< basescreen > screen_;
    int result_ { EXIT_SUCCESS };
    
protected:
    baseservice();

private:
    s_poll poll_;
    utils::scoped_thread< s_poll > poll_thread_;

    virtual void f_run() = 0;
    virtual int f_stop() = 0;
};

#endif /* BASE_H */

