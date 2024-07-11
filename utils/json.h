/*!
     \file: json.h
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

namespace js {

class object {
public:
    object() = default;

    object( const std::string &v )
        : value_( v )
    {}

    const object& operator[]( const std::string &key ) const
    {
        const auto it = subobjects_.find( key );
        if( it == subobjects_.end() )
        {
            throw std::logic_error( key + " not found");
        }
        return it->second;
    }

    void emplace( const std::string &key, const object &sub )
    {
        subobjects_[key] = sub;
    }

    void push_back( const std::string &key, const object& elem )
    {
        array_[key].push_back( elem );
    }

    operator std::string() const
    {
        return value_;
    }

    float toBool() const
    {
        return value_ == "true";
    }

    int toInt() const
    {
        return strtol( value_.c_str(), nullptr, 10 );
    }

    float toFloat() const
    {
        return strtof( value_.c_str(), nullptr );
    }

    size_t size() const
    {
        return subobjects_.size();
    }

    bool empty() const
    {
        return subobjects_.empty();
    }

    std::map< std::string, object >::iterator begin()
    {
        return subobjects_.begin();
    }

    std::map< std::string, object >::iterator end()
    {
        return subobjects_.end();
    }

    std::map< std::string, object >::const_iterator begin() const
    {
        return subobjects_.cbegin();
    }

    std::map< std::string, object >::const_iterator end() const
    {
        return subobjects_.end();
    }

    const std::map< std::string, std::vector< object > > &array() const
    {
       return array_;
    }

private:
    std::map< std::string, object > subobjects_;
    std::map< std::string, std::vector< object > > array_;
    std::string value_;
};

class parser
{
public:
    parser( const std::string &jfile );

    const object &root() const
    {
        return root_["/"];
    }

private:
    object root_;

private:
    void f_parse_object( std::string::const_iterator & it, const std::string::const_iterator &end, object &object );
};


template< typename T >
void print( T &out, const object &obj, std::string indent = "" )
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
            print( out, c.second, indent + "   " );
            out << indent << "}\n";
        }
    }
}

}  // namespace js

#endif // JSON_H
