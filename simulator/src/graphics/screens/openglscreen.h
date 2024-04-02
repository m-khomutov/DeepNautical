/* 
 * File:   base.h
 * Author: mkh
 *
 * Created on 5 февраля 2023 г., 14:25
 */

#ifndef OPENGLSCREEN_H
#define OPENGLSCREEN_H

#include <kformat/kformat.h>

#include "../scene.h"
#include <list>
#include <set>
#include <string>
#include <stdexcept>

class screen_error: public std::runtime_error {
public:
    screen_error(const std::string & what);
};

class command {
public:
    enum command_type { set_scene };

    command( command_type t )
    : t_( t )
    {}

    command_type type() const
    {
        return t_;
    }

private:
    command_type t_;
};

class openglscreen: public basescreen {
public:
    openglscreen( baseframe *frame );
    openglscreen(const openglscreen& orig) = delete;
    openglscreen &operator =(const openglscreen& orig) = delete;
    virtual ~openglscreen();

    const std::set< std::string > &scenes() const override
    {
        return scenes_;
    }
    const std::string &current_scene() const override
    {
        return *scene_iter_;
    }
    void set_scene( const std::string &scene ) override;

protected:
    std::set< std::string > scenes_;
    std::set< std::string >::iterator scene_iter_;
    std::vector< std::shared_ptr< scene > > sc_;
    utils::safeguard< std::list< command > > commands_;

protected:
    void f_exec_command();

private:
    void f_store() override;
    bool f_load( baseprotocol *proto, float duration ) override;
};

#endif /* OPENGLSCREEN_H */

