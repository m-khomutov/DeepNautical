/*!
     \file videodevice.h
     \author mkh
     \date 5 апреля 2024 г.
     \brief Заголовочный файл класса сервиса взаимодействия с платой видеозахвата.

     Данный файл содержит в себе состояние класса сервиса взаимодействия с платой видеозахвата, объявление его интерфейсов.
 */

#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/videodev2.h>

#include "baseframe.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>

class TBaseprotocol;

/*!
   \class TVideodeviceError
   \brief Класс ошибки сервиса взаимодействия с платой видеозахвата

   Производный от std::runtime_error. Собственного состояния и методов не содержит
 */
class TVideodeviceError: public std::runtime_error
{
public:
    /*!
     * \brief Конструктор класса ошибки сервиса взаимодействия с платой видеозахвата
     * \param what строковое описание ошибки
     */
    TVideodeviceError( const std::string & what );
};

/*!
     \class TVideodevice
     \brief Класс сервиса взаимодействия с платой видеозахвата.

     В процессе функционирования открывает символьный файл видеоустройства, отображает его область памяти в свою и
     с заданным периодом снимает кадр из памяти файла.
 */
class TVideodevice
{
public:
    /*!
       \brief Конструктор класса сервиса взаимодействия с платой видеозахвата
       \param path имя символьного файла видеоустройства
     */
    TVideodevice( char const *path );
    /*!
       \brief Деструктор класса сервиса взаимодействия с платой видеозахвата.
     */
    ~TVideodevice();

    /*!
       \brief предоставляет файловый дескриптор символьного файла видеоустройства
       \return файловый дескриптор символьного файла видеоустройства
     */
    operator int() const {
        return fd_;
    }

    /*!
       \brief Отправляет видеокадр абоненту
       \param proto Протокол передачи видео, испольуемый для выгрузки буфера
     */
    void send_frame( TBaseprotocol *proto );
    /*!
       \brief Определяет, вышло ли время кадра.

       Если время кадра вышло, то записывает в переменную параметр текущее время
       \param ts указатель на переменную, хранящую  текущее время
       \return Текущая длительность кадра, если она равна или превышает заданную в конструкторе, либо -1.0
     */
    float is_frame_duration_passed( TBaseframe::time_point_t *ts ) const;

    /*!
       \brief запускает процесс получения кадров от платы видеозахвата
     */
    void start_capture();
    /*!
       \brief останавливает процесс получения кадров от платы видеозахвата
     */
    void stop_capture();

private:
    //! имя символьного файла видеоустройства
    std::string filename_;
    //! файловый дескриптор символьного файла видеоустройства
    int fd_;

    //! структура свойств устройства видеозахвата
    struct v4l2_capability capabilities_;

    //! структура формата устройства видеозахвата
    struct v4l2_format v4l2_format_;
    //! структура буфера устройства видеозахвата
    struct v4l2_buffer v4l2_buffer_;
    //! формат пикселя, поддерживаемый платой видеозахвата (YVUV или RGB)
    int pixelformat_;
    //!  ширина кадра
    int width_;
    //! высота кадра
    int height_;
    //! размерность строки кадра (ширина кадра * количество цветовых каналов в пикселе)
    int bytesperline_;
    //! размер снятого кадра
    int sizeimage_;
    //! размерность пиксела в байтах (количество цветовых каналов)
    int bytesperpixel_;
    //! Указатель, отображающий память устройства видеозахвата
    void *mmap_ptr_;
    //! флаг включения режима снятия кадров с видеоустройства
    bool started_ {false};

    //! буфер снятого с видеоустройства кадра
    std::vector< uint8_t > yuyv_;

    //! объект представления видеокадра
    std::unique_ptr< TBaseframe > frame_;

private:
    /*!
       \brief выводит список свойств устройства видеозахвата
     */
    void f_show_capabilities();
    /*!
       \brief настраивает формат пиксела устройства видеозахвата
     */
    void f_set_pixel_format();
    /*!
       \brief отобразить память устройства видеозахвата на локальный указатель
     */
    void f_allocate_buffers();
    /*!
       \brief включить режим генерации кадров на устройстве видеозахвата
     */
    void f_on();
     /*!
       \brief выключить режим генерации кадров на устройстве видеозахвата
     */
    void f_off();
    /*!
       \brief снять кадр с устройства видеозахвата
       \return удача/неудача процесса
     */
    bool f_get_frame();
};

#endif // VIDEODEVICE_H
