#include "json.h"
#include <fstream>

js::parser::parser( const std::string &jfile )
{
    std::ifstream f(jfile);
    if( !f.is_open() ) {
        throw std::logic_error( jfile + " failed to be opened" );
    }
    std::string json, tp;
    // прочесть файлик
    while ( std::getline( f, tp ) )
    {
        json += tp;
    }
    f.close();

    // посимвольно разобрать
    auto end = json.cend();
    for( auto it = json.cbegin(); it != end; ++it ) {
        if( std::isprint(*it) && !std::isspace(*it) ) // рассматриваем только печатные символы. Пробелы опускаем
        {
            switch( *it )
            {
            case '{':
            {
                object subobject;
                f_parse_object( ++it, end, subobject );
                root_.emplace( "/", subobject );
                break;
             }
             case '}':
                 return;
             }
         }
     }
}

void js::parser::f_parse_object( std::string::const_iterator & it, const std::string::const_iterator &end, object &object )
{
    std::string name, value, *ptr = &name;

    for( ; it != end; ++it ) {
        if( std::isprint(*it) && !std::isspace(*it) )
        {
            switch( *it )
            {
            case '{':
            {
                js::object subobject;
                f_parse_object( ++it, end, subobject );
                object.emplace( name, subobject );

                name.clear();
                value.clear();
                break;
            }
            case '}':
                if( !value.empty() )
                {
                    object.emplace( name, js::object( value ) );
                }
                return;
            case ',':
                if( !value.empty() )
                {
                    object.emplace( name, js::object( value ) );
                }
                name.clear();
                value.clear();
                ptr = &name;
                break;
            case ':':
                ptr = &value;
                break;
            case '[': /*TODO not supported*/
            {
                js::object subobject;
                f_parse_object( ++it, end, subobject );
                object.emplace( name, subobject );
                name.clear();
                break;
            }
            case ']':
                break;
            default:
                if( *it != '"' ) {
                    *ptr += *it;
                }
            }
        }
    }
}
