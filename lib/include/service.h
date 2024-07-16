/*!
     \file baseservice.h
     \author mkh
     \date 5 февраля 2023 г.
     \brief Заголовочный файл базового класса сервиса отображения сцен, видеозахвата и обработки запросов абонентов.

     Данный файл содержит в себе состояние базового класса сервиса отображения сцен, видеозахвата и обработки запросов абонентов, объявление его интерфейсов.
 */
#ifndef BASESERVICE_H
#define BASESERVICE_H

#include "loop.h"
#include "utils.h"

namespace base
{

class screen;

class service
{
public:
    service( screen *screen, uint16_t port );
    service( char const *videodevname, uint16_t port );
    service( const service& other ) = delete;
    virtual ~service() = default;

    service &operator =( const service& other ) = delete;

    virtual void onsignal( int ) = 0;

    void start_screen();
    int stop_screen();

    void start_vdev_capture();
    void stop_vdev_capture();

protected:
    int result_ { EXIT_SUCCESS };

private:
    screen * screen_ { nullptr };
    loop loop_;

    utils::scoped_thread< loop > loop_thread_;

private:
    virtual void f_start_screen()
    {}
    virtual int f_stop_screen()
    {
        return EXIT_SUCCESS;
    }
};

} // namespace base
#endif /* BASESERVICE_H */
