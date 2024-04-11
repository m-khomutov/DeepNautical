#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/videodev2.h>

#include "../encoding/baseframe.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>

class TBaseprotocol;

class videodevice_error: public std::runtime_error
{
public:
    videodevice_error( const std::string & what );
};

class videodevice
{
public:
    videodevice( char const *path );
    ~videodevice();

    operator int() const {
        return fd_;
    }

    void load( TBaseprotocol *proto );
    float frame_duration_passed( TBaseframe::time_point_t *ts ) const;

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

    std::unique_ptr< TBaseframe > frame_;

private:
    void f_show_capabilities();
    void f_set_pixel_format();
    void f_allocate_buffers();
    void f_on();
    void f_off();
    bool f_get_frame();
};

#endif // VIDEODEVICE_H
