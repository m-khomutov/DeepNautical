/*!
     \file receiver.h
     \author mkh
     \date 26 января 2023 г.
     \brief Заголовочный файл класса клиента сетевого соединения с tcp сервером.

     Данный файл содержит в себе состояние класса клиента сетевого соединения с tcp сервером, объявление его интерфейсов.
 */
#ifndef RECEIVER_H
#define RECEIVER_H

#include "c_socket.h"

#include "utils.h"
#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

/*!
   \class TReceiverError
   \brief Класс ошибки клиента сетевого соединения с tcp сервером

   Производный от std::runtime_error. Собственного состояния и методов не содержит
 */
class TReceiverError: public std::runtime_error
{
public:
    /*!
       \brief Конструктор класса ошибки клиента сетевого соединения с tcp сервером
       \param what строковое описание ошибки
     */
    TReceiverError( const std::string & what );
};

/*!
   \class TFLVheader
   \brief Содержит поля заголовочной структуры FLV (Flash Video)
 */
class TFLVheader
{
public:
    /*!
       \brief Конструктор по умолчанию.
     */
    TFLVheader() = default;
    /*!
       \brief Конструктор получения полей из принятых от сервера данных
       \param data принятые сетевые данные
     */
    explicit TFLVheader( uint8_t const *data );

    /*!
       \brief проверяет правильность полей принятого заголовка
       \return результат проверки
     */
    bool valid() const;

private:
    //! сигнатура = 'FLV '
    char signature_[3];
    //! Версия протокола = 1
    uint8_t version_ { 1 };
    //! Флаг присутствия аудио = false
    bool audio_present_ { false };
    //! Флаг присутствия видео = true
    bool video_present_ { true };
    //! смещение информационных от начала = 9
    uint32_t data_offset_ = 0;
};

/*!
   \class TFLVtag
   \brief Содержит поля структуры тега FLV (Flash Video)
 */
class TFLVtag
{
public:
    //! размер данных, передаваемых в теге
    enum { size = 13 };

    /*!
       \brief Конструктор по умолчанию.
     */
    TFLVtag() = default;
    /*!
       \brief Конструктор получения полей из принятых от сервера данных
       \param data принятые сетевые данные
     */
    explicit TFLVtag( uint8_t const *data );

    /*!
       \brief проверяет правильность полей принятого заголовка
       \return результат проверки
     */
    bool valid() const;

    /*!
     * \brief принятый размер информационных данных
     * \return принятый размер
     */
    uint32_t data_size() const
    {
        return data_size_;
    }
    /*!
     * \brief принятая временная метка
     * \return временная метка (здесь соответствует системному времени отправителя)
     */
    uint64_t timestamp() const
    {
        return timestamp_;
    }

private:
    //! Перечисление типов принимаемых данных. Здесь ипользуется только video (9)
    enum ETagType { audio = 8, video = 9 };
    //! Перечисление типов кодеков. Здесь используется только jpeg (1)
    enum ECodecID { jpeg = 1 };

    //! Принятый тип данных
    ETagType type_ { ETagType::video };
    //! Принятый размер информационных данных
    uint32_t data_size_ { 0 };
    //! Принятая временная метка (здесь соответствует системному времени отправителя)
    uint64_t timestamp_ { 0ul };
    /* В спецификации это поле имеется. Здесь отсутствует
    uint32_t stream_id_ { 0 };
    */
    //! Принятый тип кадра. Для jpeg всегда 1
    uint8_t frame_type_ { 1 };
    //! Приянтый тип кодека
    uint8_t codec_id_ { ECodecID::jpeg };
};

class TBasedecoder;

/*!
   \class TReceiver
   \brief Класс клиента сетевого соединения с tcp сервером
 */
class TReceiver {
public:
    /*!
       \brief алиас типа колбэка, обрабатывающего вывод временной задержки
       \param временная метка, принятая в кадре FLV
     */
    using verify_callback_t = std::function< void(uint64_t) >;

    /*!
       \brief Конструктор класса клиента сетевого соединения с tcp сервером
       \param decoder Указатель на переменную объекта декодирования принятых данных
     */
    explicit TReceiver( TBasedecoder *decoder );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TReceiver( const TReceiver& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TReceiver &operator =( const TReceiver& orig ) = delete;
    /*!
       \brief Деструктор класса. Закрывает сетевое соединение
     */
    ~TReceiver();

    /*!
       \brief запускает получение видеопотока с сетевого соединения
     */
    void start_listening_network();
    /*!
       \brief Останавливает получение видеопотока с сетевого соединения
     */
    void stop_listening_network();
    /*!
       \brief Регистрирует обработчик вывода сетевой задержки
       \param cb регистрируемый обработчик
     */
    void register_verify_callback( verify_callback_t cb );

private:
    //! Запрашиваемая точка обзора
    size_t view_ {0};
    //! Указатель на переменную объекта декодирования принятых данных
    TBasedecoder *decoder_;
    //! функция вывода сетевой временной задержки
    verify_callback_t verify_callback_ {nullptr};
    //! Флаг поддержки соединения с сервером
    std::atomic< bool > running_ { true };
    //! Умный указатель на сокет сетевого соединения с сервером
    std::unique_ptr< TCsocket > connection_;
    //! Строковое представление имени хоста
    std::string server_host_;
    //! Сетевой порт сервера
    uint16_t server_port_;
    //! Текущее действие конечного автомата получения кадра в формате FLV (Flash Video)
    size_t (TReceiver::*action)(uint8_t const *, size_t);
    //! Временная метка принятого кадра
    uint64_t timestamp_ { 0ul };
    //! Значение периода ожидания при попытках повторного соединения с сервером
    int reconnect_delay_ { 0 };

private:
    /*!
       \brief начинает соединение. ОТправляем сервеору http-запрос начала трансляции видеопотока
     */
    void f_start_connection();
    /*!
       \brief принимает тег FLV (Flash Video)
       \param data указатель на буфер с принятыми сетевыми данными
       \param size размер принятых сетевым данных в буфере
       \return размер принятого тега FLV. Ожидается 13 байтов
     */
    size_t f_receive_tag( uint8_t const *data, size_t size );
    /*!
       \brief принимает тело FLV (Flash Video) кадра
       \param data указатель на буфер с принятыми сетевыми данными
       \param size размер принятых сетевым данных в буфере
       \return размер принятого тела FLV кадра
     */
    size_t f_receive_body( uint8_t const *data, size_t size );
   /*!
       \brief принимает размер FLV (Flash Video) тега
       \param data указатель на буфер с принятыми сетевыми данными
       \param size размер принятых сетевым данных в буфере
       \return размер принятого размера FLV тега. Ожидается 4 байта
     */
    size_t f_receive_size( uint8_t const *data, size_t size );
};

#endif /* RECEIVER_H */
