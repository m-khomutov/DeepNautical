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
    service( uint16_t b_port, int width, int height, int quality, int duration );
    service( const service& orig ) = delete;
    service &operator =( const service& orig ) = delete;
    ~service();

    void run( const char *shader_dir );
    void stop();

private:
    std::unique_ptr< baseframe > frame_;
    screen screen_;
    s_poll poll_;
    utils::scoped_thread< s_poll > poll_thread_;
};

#endif /* SERVICE_H */
