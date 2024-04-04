#ifndef SERVICE_H
#define SERVICE_H


#include <kformat.h>

class service: public baseservice
{
public:
    service( char const *vdevname, uint16_t port );

    ~service()
    {}

    void onsignal( int ) override
    {}

private:
    void f_run() override
    {}
    int f_stop() override
    {
        return 0;
    }
};

#endif // SERVICE_H
