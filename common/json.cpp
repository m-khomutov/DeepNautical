#include "json.h"
#include <fstream>

NJson::TParser::TParser( const std::string &jfile )
{
    std::ifstream f(jfile);
    if( !f.is_open() ) {
        throw std::logic_error( jfile + " failed to be opened" );
    }
    std::string json, tp;
    while ( std::getline( f, tp ) )
    {
        json += tp;
    }
    f.close();

    auto end = json.cend();
    for( auto it = json.cbegin(); it != end; ++it ) {
        if( std::isprint(*it) && !std::isspace(*it) )
        {
            switch( *it )
            {
            case '{':
            {
                TObject subobject;
                f_parse_object( ++it, end, subobject );
                object_.emplace( "root", subobject );
                break;
             }
             case '}':
                 return;
             }
         }
     }
}

void NJson::TParser::f_parse_object( std::string::const_iterator & it, const std::string::const_iterator &end, TObject &object )
{
    std::string name, value, *ptr = &name;

    for( ; it != end; ++it ) {
        if( std::isprint(*it) && !std::isspace(*it) )
        {
            switch( *it )
            {
            case '{':
            {
                TObject subobject;
                f_parse_object( ++it, end, subobject );
                object.emplace( name, subobject );

                name.clear();
                value.clear();
                break;
            }
            case '}':
                if( !value.empty() )
                {
                    object.emplace( name, TObject( value ) );
                }
                return;
            case ',':
                if( !value.empty() )
                {
                    object.emplace( name, TObject( value ) );
                }
                name.clear();
                value.clear();
                ptr = &name;
                break;
            case ':':
                ptr = &value;
                break;
            case '[':
            {
                //TObject subobject;
                //parse_array( ++it, end, subobject );
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
