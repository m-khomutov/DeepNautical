/*!
     \file httpapi.h
     \author mkh
     \date 2 марта 2023 г.
     \brief Заголовочный файл класса сетевого контрольного протокола формата HTTP.

     Данный файл содержит в себе состояние класса сетевого контрольного протокола формата HTTP, объявление его интерфейсов.
 */
#ifndef HTTPAPI_H
#define HTTPAPI_H

#include <kformat.h>
#include <map>
#include <string>
#include <vector>

/*!
     \class THTTPapi
     \brief Класс сетевого контрольного протокола формата HTTP.

      Состояние класса хранит:
      - указатель на объект экрана отображения сцен;
      - текущий размер переданных данных ответа на контрольный запрос;
      - буфер, ответа на контрольный запрос;

      Публичный интерфейс, объявленный в базовом классе TBaseprotocol и реализованный в этом классе, включает методы:
      - обработки принятых из сети данных (запрос на выдачу видеопотока);
      - доотправки абоненту не полностью отправленного кадра;
      - отправки видеокадра абоненту;
      - подтверждения возможности отправлять видеокадры;
      - отправки абоненту ошибки (невозможность трансляции при неверном запросе).
      - выдачи номера точки обзора, соответствующей протоколу
 */
class THTTPapi:public TBaseprotocol {
public:
    /*!
       \class TRequest
       \brief Класс представления контрольного HTTP запроса
     */
    class TRequest {
    public:
        /*!
           \brief Конструктор класса, разбирающий строку запроса HTTP
           \param s строка запроса HTTP
         */
        explicit TRequest( const std::string &s );
        /*!
           \brief Запрещенный конструктор копии.
           \param orig  Копируемый объект
         */
        TRequest( const TRequest& orig ) = delete;
        /*!
           \brief Запрещенный оператор присваивания.
           \param orig Копируемый объект
         */
        TRequest &operator =( const TRequest& orig ) = delete;
        /*!
           \brief Деструктор по умолчанию.
         */
        ~TRequest() = default;

        //! Стартовая строка
        std::string origin;
        //! Метод HTTP-запроса
        std::string method;
        //! Цель запроса
        std::string uri;
        //! Версия используемого протокола
        std::string version;
        //! HTTP-заголовки (HTTP Headers)
        std::map< std::string, std::string > headers;
    };

    /*!
       \brief Конструктор класса контрольного протокола формата HTTP.
       \param b_sock файловый дескриптор (сокет) сетевого соединения с абонентом
       \param flags флаги, выставляемые при выдаче данных в сеть
       \param screen указатель на объект хранения сцен
     */
    explicit THTTPapi( int b_sock, int flags, TBasescreen *screen );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig  Копируемый объект
     */
    THTTPapi( const THTTPapi& orig ) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
     */
    THTTPapi &operator =( const THTTPapi& orig ) = delete;
    /*!
        \brief Деструктор.
     */
    ~THTTPapi();

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
       \brief Отправляет видеокадр абоненту. Данный протокол не использует эту функцию
       \param data Указатель на отправляемый буфер данных
       \param size Размер отправляемых данных
     */
    void send_frame( const uint8_t * data, int size ) override;
    /*!
       \brief Опровергает возможность отправлять видеокадры
       \return опровержение
     */
    bool can_send_frame() const override;
    /*!
       \brief Отправляет абоненту ошибку (невозможность трансляции при неверном запросе)
     */
    void write_error() override;

private:
    //! указатель на объект экрана отображения сцен
    TBasescreen *screen_;
    //! текущий размер переданных данных ответа на контрольный запрос
    size_t sent_ { 0 };
    //! буфер, ответа на контрольный запрос
    std::vector< uint8_t > reply_;

private:
    /*!
       \brief возвращает абоненту список имеющихся сцен
     */
    void f_send_scene_list();
    /*!
      \brief возвращает абоненту текущую сцену
     */
    void f_send_current_scene();
    /*!
      \brief выставляет текущую сцену по запросу абонента
      \param scene название сцены для запуска
     */
    void f_set_current_scene( const std::string &scene );
    /*!
     * \brief генерирует ответ на контрольный запрос
     * \param data данные ответа на контрольный запрос
     * \param size размер данных ответ на контрольный запрос
     */
    void f_set_reply( uint8_t const * data, size_t size );
    /*!
     * \brief отправляет абоненту ответ на контрольный запрос
     */
    void f_reply();

};

#endif /* HTTPAPI_H */

