/*!
     \file c_socket.h
     \author mkh
     \date 25 января 2023 г.
     \brief Заголовочный файл класса сокета сетевого соединения с tcp сервером.

     Данный файл содержит в себе состояние класса сокета сетевого соединения с tcp сервером, объявление его интерфейсов.
 */
#ifndef C_SOCKET_H
#define C_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <string>
#include <stdexcept>

/*!
   \class TCsocketError
   \brief Класс ошибки сокета соединения с tcp сервером

   Производный от std::runtime_error. Собственного состояния и методов не содержит
 */
class TCsocketError: public std::runtime_error
{
public:
    /*!
       \brief Конструктор класса ошибки сокета соединения с tcp сервером
       \param what строковое описание ошибки
     */
    TCsocketError( const std::string & what );
};

/*!
     \class TCsocket
     \brief Класс сокета сетевого соединения с tcp сервером

     Создает неблокирующее соединение с tcp сервером. Принимает данные с сервера, отправляет данные на сервер
 */
class TCsocket {
public:
    /*!
     * \brief Конструктор класса. Создает соединение с удаленным tcp серверным сокетм.
     * \param addr сетевой адрес удаленного сервера
     * \param port порт удаленного сервера
     */
    TCsocket( std::string const &addr, uint16_t port );
    /*!
       \brief Запрещенный конструктор копии.
       \param orig Копируемый объект
     */
    TCsocket( const TCsocket& orig) = delete;
    /*!
       \brief Запрещенный оператор присваивания.
       \param orig Копируемый объект
       \return Собственный объект
     */
    TCsocket &operator =( const TCsocket& orig ) = delete;
    /*!
       \brief Деструктор класса. Закрывает файловый дескриптор сетевого соединения
     */
    ~TCsocket();

    /*!
       \brief возвращает файловый дескриптор сетевого соединения
     */
    operator int() const
    {
        return fd_;
    }

    /*!
       \brief отправляет данные на удаленный tcp сервер
       \param data указатель на буфер передаваемых данных
       \param size размер передаваемых данных
       \return количество реально отправленных данных
     */
    ssize_t send( uint8_t const *data, size_t size );
    /*!
       \brief принимает данные из сети. Метод неблокирующий, поэтому количество принятых данных может быть нулевым
       \param buffer буфер сохранения принимаемых данных
       \param size размер буфера сохранения принимаемых данных
       \return количество принятых данных
     */
    ssize_t receive( uint8_t *buffer, size_t size );
    
private:
    //! файловый дескриптор сетевого соединения
    int fd_;

private:
    /*!
       \brief определяет сетевой адрес хоста по имени
       \param addr строковое имя хоста
       \return определенный сетевой адрес хоста
     */
    in_addr_t f_resolve_host( std::string const &name );
};

#endif /* C_SOCKET_H */

