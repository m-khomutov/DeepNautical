#ifndef BASESCREEN_H
#define BASESCREEN_H

#include "../encoding/baseframe.h"

class basescreen
{
public:
    basescreen( baseframe *frame );
    basescreen(const basescreen& orig) = delete;
    basescreen &operator =(const basescreen& orig) = delete;
    virtual ~basescreen() = default;

    virtual void run() = 0;
    virtual void stop() = 0;

    baseframe *frame()
    {
        return frame_;
    }

protected:
    baseframe *frame_;

};

#endif // BASESCREEN_H
