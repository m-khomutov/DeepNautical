/* 
 * File:   service.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 11:45
 */

#ifndef SERVICE_H
#define SERVICE_H

#include "graphics/screen.h"
#include "server/s_poll.h"
#include "encoding/jpegframe.h"
#include "../../share/utils.h"

class service {
public:
    service();
    service( service const &orig ) = delete;
    service &operator =( service const &orig ) = delete;
    ~service();

    void run();
    void stop();

private:
    std::unique_ptr< baseframe > frame_;
    std::unique_ptr< screen > screen_;
    s_poll poll_;
    utils::scoped_thread< s_poll > poll_thread_;
};

#endif /* SERVICE_H */
