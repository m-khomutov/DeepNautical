#include "basescreen.h"

basescreen::basescreen( TBaseframe *frame )
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

bool basescreen::load( TBaseprotocol *proto )
{
    std::lock_guard< std::mutex > lk(frame_mutex_);

    return f_load( proto );
}

float basescreen::frame_duration_passed( TBaseframe::time_point_t *ts ) const
{
    return frame_->is_duration_passed( ts );
}
