/*!
     \file mjpegprotocol.h
     \author mkh
     \date 18 апреля 2024 г.
     \brief Заголовочный файл класса сетевого протокола формата MJPEG (Motion JPEG).

     Данный файл содержит в себе состояние класса сетевого протокола формата MJPEG (Motion JPEG), реализацию его интерфейсов.
 */
#ifndef MJPEGPROTOCOL_H
#define MJPEGPROTOCOL_H

#include "baseprotocol.h"

/*!
   \class TMjpegprotocol
   \brief Реализуется протокол mjpeg выдачи видеоконтента
 */
class TMjpegprotocol: public TBaseprotocol
{
public:
    /*!
       \brief Конструктор класса сетевого протокола формата MJPEG (Motion JPEG) трансляции видеокадров.
       \param b_sock файловый дескриптор (сокет) сетевого соединения с абонентом
       \param flags флаги, выставляемые при выдаче данных в сеть
       \param scene название сцены, соответствующей протоколу
     */
    TMjpegprotocol(  int b_sock, int flags, const std::string &scene );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig  Копируемый объект
     */
    TMjpegprotocol( const TMjpegprotocol& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
     */
    TMjpegprotocol & operator =( const TMjpegprotocol& orig ) = delete;
    /*!
       \brief Деструктор базового класса сетевого протокола трансляции видеокадров.
     */
    ~TMjpegprotocol() = default;


    /*!
       \brief Вызывается при получении данных из сети.
       \param data Указатель на буфер данных, принятые из сети
       \param size Размер данных, принятых из сети
     */
    void on_data( const uint8_t * data, int size ) override;
    /*!
       \brief Позволяет доотправить данные, не ушедшие сразу, вследствие перегрузки сетевых буферов
     */
    void do_write() override;
    /*!
       \brief Отправляет видеокадр абоненту
       \param data Указатель на отправляемый буфер данных
       \param size Размер отправляемых данных
     */
    void send_frame( const uint8_t * data, int size ) override;
    /*!
       \brief Подтверждает возможность отправлять видеокадры
       \return подтверждение
     */
    bool can_send_frame() const override;
    /*!
       \brief Отправляет абоненту ошибку (невозможность трансляции при неверном запросе)
     */
    void write_error() override;

private:
    //! буфер, cодержащий фрейм видеоданных в формате MJPEG
    std::vector< uint8_t > mjpeg_frame_;
    //! размер границы MJPEG фрейма
    size_t boundary_size_;
    //! флаг отправки http-заголовка
    bool header_sent_ {false};
    //! текущий размер переданных данных фрейма
    size_t sent_ { 0 };

private:
    /*!
       \brief Отправляет абоненту фрейм формата MJPEG
     */
    void f_send_frame();
};

#endif // MJPEGPROTOCOL_H
