#ifndef BASESCREEN_H
#define BASESCREEN_H

#include "../encoding/baseframe.h"
#include <mutex>
#include <set>

class basescreen
{
public:
    basescreen( baseframe *frame );
    basescreen(const basescreen& orig) = delete;
    basescreen &operator =(const basescreen& orig) = delete;
    virtual ~basescreen() = default;

    void run();
    void stop();
    void store();
    void load( baseprotocol *proto, float duration );
    float frame_duration_passed( baseframe::time_point_t *ts ) const;

    virtual const std::set< std::string > &scenes() const = 0;
    virtual const std::string &current_scene() const = 0;
    virtual void set_scene( const std::string &scene ) = 0;

protected:
    baseframe *frame_;
    std::mutex frame_mutex_;
    baseframe::time_point_t store_ts_;

private:
    virtual void f_run() = 0;
    virtual void f_stop() = 0;
    virtual void f_store() = 0;
    virtual void f_load( baseprotocol *proto, float duration ) = 0;
};

#endif // BASESCREEN_H
