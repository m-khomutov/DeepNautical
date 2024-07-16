/*!
     \file basescreen.h
     \author mkh
     \date 25 января 2023 г.
     \brief Заголовочный файл базового класса экрана отображения сцен.

     Данный файл содержит в себе состояние базового класса экрана отображения сцен, объявление его интерфейсов.
 */
#ifndef BASESCREEN_H
#define BASESCREEN_H

#include "frame.h"

#include <mutex>
#include <set>
#include <vector>

namespace base
{

class screen
{
public:
    screen( frame *fr );
    screen( const screen& other ) = delete;
    virtual ~screen() = default;

    screen &operator =( const screen& other ) = delete;

    void run();
    void stop();

    void update_frame();
    bool send_frame( protocol *pr );
    float frame_expired( frame::time_point_t *ts ) const;

    virtual const std::set< std::string > &get_scenes() const = 0;
    virtual std::vector< std::string > current_scenes() = 0;
    virtual void set_scene( const std::string &scene, size_t position ) = 0;

protected:
    frame *frame_;
    std::mutex mutex_;
    frame::time_point_t store_ts_;
    size_t view_count_ {1};

private:
    virtual void f_run() = 0;
    virtual void f_stop() = 0;
    virtual void f_update_frame() = 0;
    virtual bool f_send_frame( protocol *pr ) = 0;
};

}  // namespace base

#endif // BASESCREEN_H
