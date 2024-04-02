#include "basescreen.h"

basescreen::basescreen( baseframe *frame )
: frame_( frame )
, store_ts_( std::chrono::high_resolution_clock::now() )
{}

void basescreen::run()
{
    f_run();
}

void basescreen::stop()
{
    f_stop();
}

void basescreen::store()
{
    std::lock_guard< std::mutex > lk(frame_mutex_);

    f_store();
}

bool basescreen::load( baseprotocol *proto, float duration )
{
    std::lock_guard< std::mutex > lk(frame_mutex_);

    return f_load( proto, duration );
}

float basescreen::frame_duration_passed(baseframe::time_point_t *ts) const
{
    return frame_->duration_passed( ts );
}
