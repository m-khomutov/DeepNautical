#include "screen.h"

base::screen::creen( base::frame *fr )
    : frame_( fr )
    , store_ts_( std::chrono::high_resolution_clock::now() )
{}

void base::screen::run()
{
    f_run();
}

void base::screen::stop()
{
    f_stop();
}

void base::screen::update_frame()
{
    std::lock_guard< std::mutex > lk(mutex_);

    f_update_frame();
}

bool base::screen::send_frame( base::proto *p )
{
    std::lock_guard< std::mutex > lk(mutex_);

    return f_send_frame( proto );
}

float base::screen::frame_expired( base::frame::time_point_t *ts ) const
{
    return frame_->expired( ts );
}
