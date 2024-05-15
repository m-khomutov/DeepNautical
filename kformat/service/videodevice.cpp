#include "videodevice.h"
#include "jpegframe.h"
#include "utils.h"

#include <sys/mman.h>


namespace {

// ограничивает значение диапазоном 0 - 255
int clip( int val )
{
    return val < 0 ? 0 : (val > 255 ? 255 : val);
}

// перевод пиксела из YUV формата цвета в RGB
void yuv_to_rgb_pixel( int y, int u, int v, uint8_t *rgb )
{
    float r = y + 1.4065 * (v - 128);
    float g = y - 0.3455 * (u - 128) - 0.7169 * (v - 128);
    float b = y + 1.1790 * (u - 128);

    rgb[0] = uint8_t(clip( r ));
    rgb[1] = uint8_t(clip( g ));
    rgb[2] = uint8_t(clip( b ));
}

// перевод пиксела из YVUV формата цвета в RGB
void yuyv_to_rgb_pixel(unsigned char *yuyv, unsigned char *rgb)
{
    yuv_to_rgb_pixel( yuyv[0], yuyv[1], yuyv[3], &rgb[0] ); //first pixel
    yuv_to_rgb_pixel( yuyv[1], yuyv[2], yuyv[3], &rgb[3] ); //second pixel
}

// перевод кадра из YVUV формата цвета в RGB
void yuyv_to_rgb( uint8_t *yuyv, uint8_t *rgb, int width, int height )
{
    long yuv_size = width * height * 2;
    long rgb_size = width * height * 3;

    for (int i(0), j(0); i < rgb_size && j < yuv_size; i += 6, j += 4 )
    {
        yuyv_to_rgb_pixel( &yuyv[j], &rgb[i] );
    }
}

}  // namespace


TVideodeviceError::TVideodeviceError( const std::string &what )
: std::runtime_error( what + std::string(": ") + std::string(strerror( errno )) )
{}



TVideodevice::TVideodevice( char const *path )
: filename_( path )
, fd_( open( path, O_RDWR ) )
{
    if( fd_ < 0 ) {
        throw TVideodeviceError( strerror(errno) );
    }

    // далее настройка драйвера устройства видеозахвата

    int rc = ::ioctl( fd_, VIDIOC_QUERYCAP, &capabilities_ );
    if( rc < 0 ) {
        ::close( fd_ );
        throw TVideodeviceError( strerror(errno) );
    }

    f_show_capabilities();
    try {
        f_set_pixel_format();
        f_allocate_buffers();
        f_on();
    }
    catch ( ... ) {
        ::close( fd_ );
        throw;
    }
}

TVideodevice::~TVideodevice()
{
    try {
        f_off();
    }
    catch ( ... ) {

    }
    ::close( fd_ );
}

void TVideodevice::start_capture()
{
    started_ = true;
}

void TVideodevice::stop_capture()
{
    started_ = false;
}

void TVideodevice::send_frame( TBaseprotocol *proto )
{
    if( started_ && f_get_frame() )
    {
        frame_->send_buffer( proto );
    }
}

float TVideodevice::is_frame_duration_passed() const
{
    return started_ ? frame_->is_duration_passed() : -1.f;
}

void TVideodevice::f_on()
{
  if( ioctl( fd_, VIDIOC_STREAMON, &v4l2_format_.type ) < 0 )
      throw TVideodeviceError( std::string("VIDIOC_STREAMON error: ") + strerror(errno) );
}

void TVideodevice::f_off()
{
    if( ioctl( fd_, VIDIOC_STREAMOFF, &v4l2_format_.type ) < 0 )
        throw TVideodeviceError( std::string("VIDIOC_STREAMOFF error: ") + strerror(errno) );
}

void TVideodevice::f_show_capabilities()
{
    fprintf( stderr, "\t%s:\n\n", filename_.c_str() );
    fprintf( stderr, "Driver: %s\n", capabilities_.driver );
    fprintf( stderr, "Version: %u.%u.%u\n", (capabilities_.version >> 16) & 0xFF,
                                            (capabilities_.version >> 8) & 0xFF,
                                            (capabilities_.version) & 0xFF );
    fprintf( stderr, "Card: %s\n", capabilities_.card );
    fprintf( stderr, "Bus info: %s\n", capabilities_.bus_info );
    fprintf( stderr, "Capabilities: 0x%08X\n", capabilities_.capabilities );

    int caps = capabilities_.capabilities;
    if( caps & V4L2_CAP_VIDEO_CAPTURE )
        fprintf( stderr, "    0x00000001 - V4L2_CAP_VIDEO_CAPTURE\n"
                         "      The device supports the single-planar API through the Video Capture interface.\n" );
    if( caps & V4L2_CAP_VIDEO_OUTPUT )
        fprintf(stderr, "    0x00000002 - V4L2_CAP_VIDEO_OUTPUT\n"
                        "      The device supports the single-planar API through the Video Output interface.\n" );
    if( caps & V4L2_CAP_VIDEO_OVERLAY )
        fprintf(stderr, "    0x00000004 - V4L2_CAP_VIDEO_OVERLAY\n"
                        "      The device supports the Video Overlay interface.\n");
    if( caps & 0x00000008 )
        fprintf( stderr, "    0x00000008 - **unknown flag**\n"
                         "      This one isn't in the documentation.\n");
    if( caps & V4L2_CAP_VBI_CAPTURE )
        fprintf( stderr, "    0x00000010 - V4L2_CAP_VBI_CAPTURE\n"
                         "      The device supports the Raw VBI Capture interface.\n" );
    if( caps & V4L2_CAP_VBI_OUTPUT )
        fprintf( stderr, "    0x00000020 - V4L2_CAP_VBI_OUTPUT\n"
                         "      The device supports the Raw VBI Output interface.\n" );
    if( caps & V4L2_CAP_SLICED_VBI_CAPTURE )
        fprintf( stderr, "    0x00000040 - V4L2_CAP_SLICED_VBI_CAPTURE\n"
                         "      The device supports the Sliced VBI Capture interface.\n");
    if( caps & V4L2_CAP_SLICED_VBI_OUTPUT )
        fprintf( stderr, "    0x00000080 - V4L2_CAP_SLICED_VBI_OUTPUT\n"
                         "      The device supports the Sliced VBI Output interface.\n");
    if( caps & V4L2_CAP_RDS_CAPTURE )
        fprintf( stderr, "    0x00000100 - V4L2_CAP_RDS_CAPTURE\n"
                         "      The device supports the RDS capture interface.\n");
    if( caps & V4L2_CAP_VIDEO_OUTPUT_OVERLAY )
        fprintf( stderr, "    0x00000200 - V4L2_CAP_VIDEO_OUTPUT_OVERLAY\n"
                         "      The device supports the Video Output Overlay (OSD) interface.\n");
    if( caps & V4L2_CAP_HW_FREQ_SEEK )
        fprintf( stderr, "    0x00000400 - V4L2_CAP_HW_FREQ_SEEK\n"
                         "      The device supports the ioctl VIDIOC_S_HW_FREQ_SEEK ioctl for hardware frequency seeking.\n");
    if( caps & V4L2_CAP_RDS_OUTPUT )
        fprintf( stderr, "    0x00000800 - V4L2_CAP_RDS_OUTPUT\n"
                         "      The device supports the RDS output interface.\n");
    if( caps & V4L2_CAP_VIDEO_CAPTURE_MPLANE )
        fprintf( stderr, "    0x00001000 - V4L2_CAP_VIDEO_CAPTURE_MPLANE\n"
                         "      The device supports the multi-planar API through the Video Capture interface.\n");
    if( caps & V4L2_CAP_VIDEO_OUTPUT_MPLANE )
        fprintf( stderr, "    0x00002000 - V4L2_CAP_VIDEO_OUTPUT_MPLANE\n"
                         "      The device supports the multi-planar API through the Video Output interface.\n");
    if( caps & V4L2_CAP_VIDEO_M2M )
        fprintf( stderr, "    0x00004000 - V4L2_CAP_VIDEO_M2M\n"
                         "      The device supports the single-planar API through the Video Memory-To-Memory interface.\n");
    if( caps & V4L2_CAP_VIDEO_M2M_MPLANE )
        fprintf( stderr, "    0x00008000 - V4L2_CAP_VIDEO_M2M_MPLANE\n"
                         "      The device supports the multi-planar API through the Video Memory-To-Memory interface.\n");
    if( caps & V4L2_CAP_TUNER )
        fprintf( stderr, "    0x00010000 - V4L2_CAP_TUNER\n"
                         "      The device has some sort of tuner to receive RF-modulated video signals.\n");
    if( caps & V4L2_CAP_AUDIO )
        fprintf( stderr, "    0x00020000 - V4L2_CAP_AUDIO\n"
                         "      The device has audio inputs or outputs. It may or may not support audio recording or playback, in PCM or compressed formats.\n");
    if( caps & V4L2_CAP_RADIO )
        fprintf( stderr, "    0x00040000 - V4L2_CAP_RADIO\n"
                         "      This is a radio receiver.\n");
    if( caps & V4L2_CAP_MODULATOR )
        fprintf( stderr, "    0x00080000 - V4L2_CAP_MODULATOR\n"
                         "      The device has some sort of modulator to emit RF-modulated video/audio signals.\n");
    if( caps & V4L2_CAP_SDR_CAPTURE )
        fprintf( stderr, "    0x00100000 - V4L2_CAP_SDR_CAPTURE\n"
                         "      The device supports the SDR Capture interface.\n");
    if( caps & V4L2_CAP_EXT_PIX_FORMAT )
        fprintf(stderr, "    0x00200000 - V4L2_CAP_EXT_PIX_FORMAT\n"
                        "      The device supports the struct v4l2_pix_format extended fields.\n");
    if( caps & V4L2_CAP_SDR_OUTPUT )
        fprintf( stderr, "    0x00400000 - V4L2_CAP_SDR_OUTPUT\n"
                         "      The device supports the SDR Output interface.\n");
    if( caps & V4L2_CAP_META_CAPTURE )
        fprintf( stderr, "    0x00800000 - V4L2_CAP_META_CAPTURE\n"
                         "      The device supports the Metadata Interface capture interface.\n");
    if( caps & V4L2_CAP_READWRITE )
        fprintf( stderr, "    0x01000000 - V4L2_CAP_READWRITE\n"
                         "      The device supports the read() and/or write() I/O methods.\n");
    if( caps & V4L2_CAP_ASYNCIO )
        fprintf( stderr, "    0x02000000 - V4L2_CAP_ASYNCIO\n"
                         "      The device supports the asynchronous I/O methods.\n");
    if( caps & V4L2_CAP_STREAMING )
        fprintf( stderr, "    0x04000000 - V4L2_CAP_STREAMING\n"
                         "      The device supports the streaming I/O method.\n");
    if( caps & V4L2_CAP_META_OUTPUT )
        fprintf( stderr, "    0x08000000 - V4L2_CAP_META_OUTPUT\n"
                         "      The device supports the Metadata Interface output interface.\n");
    if( caps & V4L2_CAP_TOUCH )
        fprintf( stderr, "    0x10000000 - V4L2_CAP_TOUCH\n"
                         "      This is a touch device.\n");
    if( caps & 0x20000000 )
        fprintf( stderr, "    0x20000000 - V4L2_CAP_IO_MC\n"
                         "      There is only one input and/or output seen from userspace. The whole video topology configuration, including which I/O entity is routed to the input/output, is configured by userspace via the Media Controller.\n");
    if( caps & 0x40000000 )
        fprintf( stderr, "    0x40000000 - **unknown flag**\n"
                         "      This one isn't in the documentation.\n");
    if( caps & V4L2_CAP_DEVICE_CAPS )
        fprintf( stderr, "    0x80000000 - V4L2_CAP_DEVICE_CAPS\n"
                         "      The driver fills the device_caps field.\n");
    if( caps & V4L2_CAP_VIDEO_M2M ) {
        fprintf( stderr, "\nWill use single-plane types for input and output.\n\n" );
        v4l2_format_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    } else if( caps & V4L2_CAP_VIDEO_M2M_MPLANE ) {
        fprintf( stderr, "\nWill use multi-plane types for input and output.\n\n" );
        v4l2_format_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    } else {
        if( caps & V4L2_CAP_VIDEO_CAPTURE ) {
            fprintf( stderr, "  Will use single-plane type for input.\n" );
            v4l2_format_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        } else if (caps & V4L2_CAP_VIDEO_CAPTURE_MPLANE) {
            fprintf( stderr, "  Will use multi-plane type for input.\n" );
            v4l2_format_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        };
    }
}

void TVideodevice::f_set_pixel_format()
{
    int width = 720;
    int height = 576;
    int pixelformat = V4L2_PIX_FMT_RGB24;
    int type = v4l2_format_.type;

    memset( &v4l2_format_, 0, sizeof(v4l2_format) );
    v4l2_format_.type = type;
    v4l2_format_.fmt.pix.pixelformat = pixelformat;
    v4l2_format_.fmt.pix.width = width;
    v4l2_format_.fmt.pix.height = height;
    v4l2_format_.fmt.pix.field = V4L2_FIELD_NONE;

    if( ioctl( fd_, VIDIOC_S_FMT, &v4l2_format_ ) < 0 )
    {
        throw TVideodeviceError( std::string("ioctl(VIDIOC_S_FMT) returned ") + strerror(errno) );
    }

    pixelformat_ = v4l2_format_.fmt.pix.pixelformat;
    width_ = v4l2_format_.fmt.pix.width;
    height_ = v4l2_format_.fmt.pix.height;
    bytesperline_ = v4l2_format_.fmt.pix.bytesperline;
    sizeimage_ = v4l2_format_.fmt.pix.sizeimage;
    bytesperpixel_ = bytesperline_ / width_;

    if( pixelformat_ != pixelformat || width_ != width || height_ != height)
    {
        long text = pixelformat;
        fprintf( stderr, "Requested format: %s, %d x %d\n", (char *) &text, width, height );
    }
    long text = pixelformat_;
    fprintf( stderr, "Captured format: %s, %d x %d\n", (char *) &text, width_, height_ );
}

void TVideodevice::f_allocate_buffers()
{
    struct v4l2_requestbuffers request {};

    request.type = v4l2_format_.type;
    request.memory = V4L2_MEMORY_MMAP;
    request.count = 1;
    if( ioctl( fd_, VIDIOC_REQBUFS, &request ) < 0 ) {
        throw TVideodeviceError( std::string("ioctl(VIDIOC_REQBUFS): ") + strerror(errno) );
    }

    memset( &v4l2_buffer_, 0, sizeof(v4l2_buffer_) );
    v4l2_buffer_.type = v4l2_format_.type;
    v4l2_buffer_.memory = V4L2_MEMORY_MMAP;
    v4l2_buffer_.index = 0;

    if( ioctl( fd_, VIDIOC_QUERYBUF, &v4l2_buffer_ ) < 0 )
        throw TVideodeviceError( std::string("ioctl(VIDIOC_QUERYBUF): ") + strerror(errno) );

    long offset = v4l2_buffer_.m.offset;
    mmap_ptr_ = mmap( NULL, v4l2_buffer_.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, offset );

    if( mmap_ptr_ == MAP_FAILED )
    {
        throw TVideodeviceError( std::string("mmap: ") + strerror(errno) );
    }

    printf("allocated buffer at %p of size %d from offset 0x%08lx\n", mmap_ptr_,
                                                                      v4l2_buffer_.length,
                                                                      offset );
    frame_.reset( new TJpegframe( NUtils::TGeometry( width_, height_ ), 100, 10, false ) );
}

bool TVideodevice::f_get_frame()
{
    if( ioctl( fd_, VIDIOC_QBUF, &v4l2_buffer_ ) < 0 ) {
        perror("Could not queue buffer, VIDIOC_QBUF");
        return false;
    }
    if( ioctl( fd_, VIDIOC_DQBUF, &v4l2_buffer_ ) < 0 ) {
        perror("Could not dequeue the buffer, VIDIOC_DQBUF");
        return false;
    }

    if( pixelformat_ == V4L2_PIX_FMT_YUYV )
    {
        yuyv_.resize( v4l2_buffer_.bytesused );
        ::memcpy( yuyv_.data(), (uint8_t*)mmap_ptr_, yuyv_.size() );

        yuyv_to_rgb( yuyv_.data(), frame_->buffer( width_, height_ ), width_, height_ );
    }
    else if( pixelformat_ == V4L2_PIX_FMT_RGB24 )
    {
        ::memcpy( frame_->buffer( width_, height_ ), (uint8_t*)mmap_ptr_, v4l2_buffer_.bytesused );
    }
    frame_->prepare_buffer();

    return true;
}
