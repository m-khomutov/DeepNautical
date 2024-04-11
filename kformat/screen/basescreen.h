#ifndef BASESCREEN_H
#define BASESCREEN_H

#include "../encoding/baseframe.h"
#include <mutex>
#include <set>

class basescreen
{
public:
    basescreen( TBaseframe *frame );
    basescreen(const basescreen& orig) = delete;
    basescreen &operator =(const basescreen& orig) = delete;
    virtual ~basescreen() = default;

    void run();
    void stop();
    void store();
    bool load( TBaseprotocol *proto );
    float frame_duration_passed( TBaseframe::time_point_t *ts ) const;

    virtual const std::set< std::string > &scenes() const = 0;
    virtual const std::string &current_scene() const = 0;
    virtual void set_scene( const std::string &scene ) = 0;

protected:
    TBaseframe *frame_;
    std::mutex frame_mutex_;
    TBaseframe::time_point_t store_ts_;
    size_t viewes_ {1};

private:
    virtual void f_run() = 0;
    virtual void f_stop() = 0;
    virtual void f_store() = 0;
    virtual bool f_load( TBaseprotocol *proto ) = 0;
};

#endif // BASESCREEN_H
