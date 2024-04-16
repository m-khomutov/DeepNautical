/* 
 * File:   baseviewer.h
 * Author: mkh
 *
 * Created on 6 февраля 2023 г., 16:09
 */

#ifndef BASEVIEWER_H
#define BASEVIEWER_H

#include "client/receiver.h"
#include "decoder/basedecoder.h"
#include "utils.h"

class basedecoder;

class viewer_error: public std::runtime_error
{
public:
    viewer_error( const std::string & what );
};

class baseviewer {
public:
    baseviewer();
    baseviewer(const baseviewer& orig) = delete;
    baseviewer &operator =(const baseviewer& orig) = delete;
    virtual ~baseviewer();

    virtual void onsignal( int ) = 0;

    void start_stream();
    int stop_stream();
    /*!
       \brief Регистрирует обработчик вывода сетевой задержки
       \param cb регистрируемый обработчик
     */
    void register_verify_callback( TReceiver::verify_callback_t cb );

protected:
    NUtils::TImage frame_;
    std::unique_ptr< basedecoder > decoder_;
    TReceiver receiver_;
    NUtils::TScopedThread< TReceiver > rec_thread_;
    int update_tag_ { -1 }; 

private:
    virtual void f_start_stream() = 0;
    virtual int f_stop_stream() = 0;

};

#endif /* BASEVIEWER_H */
