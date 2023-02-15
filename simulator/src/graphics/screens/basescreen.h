/* 
 * File:   base.h
 * Author: mkh
 *
 * Created on 5 февраля 2023 г., 14:25
 */

#ifndef BASESCREEN_H
#define BASESCREEN_H

#include "../../encoding/jpegframe.h"
#include <string>
#include <stdexcept>

class screen_error: public std::runtime_error {
public:
    screen_error(const std::string & what);
};

class basescreen {
public:
    basescreen();
    basescreen(const basescreen& orig) = delete;
    basescreen &operator =(const basescreen& orig) = delete;
    virtual ~basescreen();

    virtual void run() = 0;
    virtual void stop() = 0;
private:

};

#endif /* BASESCREEN_H */

