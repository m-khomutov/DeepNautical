/*!
     \file:   json.h
     \author: mkh
     \date 7 мая 2024 г.
     \brief Заголовочный файл парсера json-формата.

     Данный файл содержит в себе класс упрощенного парсера json-формата.
 */
#ifndef JSON_H
#define JSON_H

#include <stdexcept>
#include <map>
#include <string>
#include <vector>

namespace NJson {

/*!
   \class TObject
   \brief Класс, содержащий поля структуры json
 */
class TObject {
public:
    /*!
       \brief Конструктор по умолчанию
     */
    TObject()
    {}

    /*!
       \brief Конструктор преобразования из строки. Сохраняет строку как значение
       \param v строка преобразования
     */
    TObject( const std::string &v )
    : value_( v )
    {}

    /*!
       \brief operator [] оператор индекса. Ищет объект в подобъектах по ключу
       \param key ключ поиска
       \return найденный объект или свой собственный
     */
    const TObject& operator[]( const std::string &key ) const
    {
        const auto it = subobjects_.find( key );
        if( it == subobjects_.end() )
        {
            throw std::logic_error( key + " not found");
        }
        return it->second;
    }

    /*!
       \brief emplace добавляет объект в подобъекты
       \param key ключ добавляемого объекта
       \param sub добавляемый объект
     */
    void emplace( const std::string &name, const TObject &sub )
    {
        subobjects_[name] = sub;
    }

    /*!
       \brief push_back добавляет объект в массив объектов
       \param key ключ массива добавляемого объекта
       \param elem добавляемый объект
     */
    void push_back( const std::string &key, const TObject& elem )
    {
        array_[key].push_back( elem );
    }

    /*!
       \brief operator std::string оператор неявного преобразования к типу std::string. Возвращает значение
     */
    operator std::string() const
    {
        return value_;
    }

    /*!
       \brief метод приведения значения к типу boolean. Возвращает значение, приведенное к boolean
       \return значение как boolean
     */
    float toBool() const
    {
        return value_ == "true";
    }

    /*!
       \brief метод приведения значения к типу int. Возвращает значение, приведенное к int
       \return значение как int
     */
    int toInt() const
    {
        return strtol( value_.c_str(), nullptr, 10 );
    }

    /*!
       \brief метод приведения значения к типу float. Возвращает значение, приведенное к float
       \return значение как float
     */
    float toFloat() const
    {
        return strtof( value_.c_str(), nullptr );
    }

    /*!
       \brief size возвращает количество подобъектов
       \return количество подобъектов
     */
    size_t size() const
    {
        return subobjects_.size();
    }

    /*!
       \brief empty проверяет наличие подобъектов
       \return результат проверки
     */
    bool empty() const
    {
        return subobjects_.empty();
    }

    /*!
       \brief begin определяет начало диапазона подобъектов
       \return начало диапазона подобъектов
     */
    std::map< std::string, TObject >::iterator begin()
    {
        return subobjects_.begin();
    }
    /*!
       \brief end определяет конец диапазона подобъектов
       \return конец диапазона подобъектов
     */
    std::map< std::string, TObject >::iterator end()
    {
        return subobjects_.end();
    }
    /*!
       \brief begin определяет константное начало диапазона подобъектов
       \return константное начало диапазона подобъектов
     */
    std::map< std::string, TObject >::const_iterator begin() const
    {
        return subobjects_.cbegin();
    }
    /*!
       \brief end определяет константный конец диапазона подобъектов
       \return константный конец диапазона подобъектов
     */
    std::map< std::string, TObject >::const_iterator end() const
    {
        return subobjects_.end();
    }

    const std::map< std::string, std::vector< TObject > > &array() const
    {
       return array_;
    }

private:
    std::map< std::string, TObject > subobjects_;
    std::map< std::string, std::vector< TObject > > array_;
    std::string value_;
};

/*!
   \class TParser
   \brief Разбирает структуру json. Хранит объект полей структуры
 */
class TParser
{
public:
    /*!
       \brief TParser Конструктор класса
       \param json строка формата json
     */
    TParser( const std::string &jfile );

    /*!
     * \brief json возвращает объект полей json
     * \return объект
     */
    const TObject &json() const
    {
        return object_["/"];
    }

private:
    //! объект хранения полей json
    TObject object_;

private:
    /*!
       \brief f_parse_object разбирает объект формата json
       \param it итератор начала разбора
       \param end итератор конца строки json
       \param object объект сохранения
     */
    void f_parse_object( std::string::const_iterator & it, const std::string::const_iterator &end, TObject &object );
};


/*!
   \brief выводит объект JSON структуры в объект представления
   \param out объект представления
   \param indent текущий отступ для простоты восприятия структуры JSON
 */
template< typename STR >
void reprObject( STR &out, const TObject &obj, std::string indent = "" )
{
    for( const auto &c : obj )
    {
        out << indent << c.first;
        if( c.second.empty() )
        {
            out << ": " << std::string(c.second) << "\n";
        }
        else
        {
            out << ": {\n";
            reprObject( out, c.second, indent + "   " );
            out << indent << "}\n";
        }
        /*for( const auto &a : obj.array() ) {
            out << a.first << "[ ";
            reprObject( out, a.second, indent );
            out << " ]\n";
        }*/
    }
}

}  // namespace NJson

#endif // JSON_H
