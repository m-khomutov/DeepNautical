/* 
 * File:   avi.h
 * Author: mkh
 *
 * Created on 12 февраля 2023 г., 14:42
 */

#ifndef AVI_H
#define AVI_H

#include "kformat/utils.h"
#include <fstream>
#include <stdexcept>
#include <memory>
#include <vector>

class avi_error: public std::runtime_error {
public:
    avi_error(const std::string & what);
};

class avi {
public:
    struct frame {
    public:
        frame() = default;
        explicit frame( FILE *f );

        bool valid( FILE *f ) const;

        size_t begin { 0 };
        size_t size { 0 };
    };
    using frame_t = std::vector< frame >;
    
    class atom {
    public:
        enum type: uint32_t {
            riff = 0x46464952,
            lst  = 0x5453494c,
            avih = 0x68697661,
            strh = 0x68727473,
            strf = 0x66727473,
            movi = 0x69766f6d,
        };

        static std::unique_ptr< atom > make( FILE *f, frame_t *frames );

        explicit atom( uint32_t fourcc ) : fourcc_( fourcc )
        {}
        atom(const atom& orig) = delete;
        atom &operator =(const atom& orig) = delete;
        virtual ~atom() = default;

        uint32_t fourcc() const
        {
            return fourcc_;
        };
        uint32_t size() const
        {
            return size_;
        };
        std::string to_str() const;

    protected:
        uint32_t fourcc_;
        uint32_t size_;
    };
    
    class avih: public atom {
    public:
        avih( FILE *f, frame_t *frames );

        uint32_t framerate_mcs; // frame display rate (or 0)
        uint32_t transferrate; // max. transfer rate
        uint32_t padding; // pad to multiples of this size
        uint32_t flags; // the ever-present flags
        uint32_t frame_count; // # frames in file
        uint32_t initial_frames;
        uint32_t streams;
        uint32_t suggested_bufsize;
        uint32_t width;
        uint32_t height;
    };
    
    class strh: public atom {
    public:
        explicit strh( FILE *f );

        uint32_t fcc_type;
        uint32_t fcc_handler;
        uint32_t flags;
        uint16_t priority;
        uint16_t language;
        uint32_t initial_frames;
        uint32_t scale;
        uint32_t rate; /* dwRate / dwScale == samples/second */
        uint32_t start;
        uint32_t length; /* In units above... */
        uint32_t suggested_bufsize;
        uint32_t quality;
        uint32_t samplesize;
        uint32_t frame[ 4 ];
    };
    
    class strf: public atom {
    public:
        explicit strf( FILE *f );

        uint32_t bisize; 
        uint32_t width; //LONG
        uint32_t height; //LONG
        uint16_t planes; 
        uint16_t bitcount; 
        uint32_t compression; 
        uint32_t sizeimage; 
        uint32_t xppm; //LONG
        uint32_t yppm; //LONG
        uint32_t clrused; 
        uint32_t clrimportant; 
    };
    
    class list: public atom {
    public:
        list( atom::type type, FILE *f, frame_t *frames );

    private:
        atom::type type_;
        uint32_t list_;
    };

   
    explicit avi( char const *filename );
    avi(const avi& orig) = delete;
    avi operator =(const avi& orig) = delete;
    ~avi();

    NUtils::image &next_image();

    double frame_duration() const
    {
        return frame_duration_;
    }
    
private:
    struct clfl { void operator()( FILE *f ) { fclose( f ); } };
    std::unique_ptr< FILE, clfl > ifile_;

    frame_t frames_;
    frame_t::iterator frame_iter_; 
    std::vector< uint8_t > jpeg;
    NUtils::jpeg_codec codec_;
    NUtils::image image_;
    double frame_duration_ {0.0f };
};

#endif /* AVI_H */
