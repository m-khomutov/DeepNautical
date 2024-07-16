/*!
     \file videodevice.h
     \author mkh
     \date 5 апреля 2024 г.
     \brief Заголовочный файл класса сервиса взаимодействия с платой видеозахвата.

     Данный файл содержит в себе состояние класса сервиса взаимодействия с платой видеозахвата, объявление его интерфейсов.
 */

#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

#include "frame.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/videodev2.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>

namespace base
{

class protocol;

} // namespace base

class vdeverror: public std::runtime_error
{
public:
    vdeverror( const std::string & what );
};

class vdev
{
public:
    vdev( char const *path );
    ~vdev();

    operator int() const
    {
        return fd_;
    }

    void send_frame( base::protocol *p );
    float frame_expired( base::frame::time_point_t *ts ) const;

    void start();
    void stop();

private:
    std::string filename_;
    int fd_;

    struct v4l2_capability capabilities_;
    struct v4l2_format v4l2_format_;
    struct v4l2_buffer v4l2_buffer_;

    int pixelformat_;
    int width_;
    int height_;
    int bytesperline_;
    int sizeimage_;
    int bytesperpixel_;

    void *mmap_ptr_;

    bool started_ {false};

    std::vector< uint8_t > yuyv_;

    std::unique_ptr< base::frame > frame_;

private:
    void f_show_capabilities();
    void f_set_pixel_format();
    void f_allocate_buffers();
    void f_enable();
    void f_disable();

    bool f_get_frame();
};

#endif // VIDEODEVICE_H
