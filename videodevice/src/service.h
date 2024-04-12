#ifndef SERVICE_H
#define SERVICE_H


#include <kformat.h>

class service: public TBaseservice
{
public:
    service( char const *vdevname, uint16_t port );

    ~service()
    {}

    void onsignal( int ) override
    {}
};

#endif // SERVICE_H
