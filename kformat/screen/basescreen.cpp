#include "basescreen.h"

TBasescreen::TBasescreen()
{}

void TBasescreen::run_scene_display()
{
    f_run_scene_display();
}

void TBasescreen::stop_scene_display()
{
    f_stop_scene_display();
}

void TBasescreen::send_stored_scene_frame( TBaseprotocol *proto )
{
    f_send_stored_scene_frame( proto );
}
