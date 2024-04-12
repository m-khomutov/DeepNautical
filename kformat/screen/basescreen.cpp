#include "basescreen.h"

TBasescreen::TBasescreen( TBaseframe *frame )
: frame_( frame )
, store_ts_( std::chrono::high_resolution_clock::now() )
{}

void TBasescreen::run_scene_display()
{
    f_run_scene_display();
}

void TBasescreen::stop_scene_display()
{
    f_stop_scene_display();
}

void TBasescreen::store_scene_frame()
{
    std::lock_guard< std::mutex > lk(frame_mutex_);

    f_store_scene_frame();
}

bool TBasescreen::send_stored_scene_frame( TBaseprotocol *proto )
{
    std::lock_guard< std::mutex > lk(frame_mutex_);

    return f_send_stored_scene_frame( proto );
}

float TBasescreen::is_frame_duration_passed( TBaseframe::time_point_t *ts ) const
{
    return frame_->is_duration_passed( ts );
}
