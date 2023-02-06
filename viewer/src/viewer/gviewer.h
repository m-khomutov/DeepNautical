/* 
 * File:   viewer.h
 * Author: mkh
 *
 * Created on 25 января 2023 г., 17:49
 */

#ifndef VIEWER_H
#define VIEWER_H

#include "baseviewer.h"
#include <gtk/gtk.h>
#include <stdexcept>

class g_viewer: public baseviewer {
public:
    g_viewer( int argc, char *argv[] );
    g_viewer(const g_viewer& orig) = delete;
    g_viewer &operator =(const g_viewer& orig) = delete;
    ~g_viewer() = default;

    void onsignal( int ) override;
    void update();

private:
    struct del { void operator ()(GtkAllocation *p) { g_free(p); } };
    
    GtkWidget *window_;
    GtkWidget *layout_; 
    GtkWidget *image_ = nullptr;
    std::unique_ptr< GtkAllocation, del > allocation_;

private:
    void f_run() override;
    int f_stop() override;
};

#endif /* VIEWER_H */
