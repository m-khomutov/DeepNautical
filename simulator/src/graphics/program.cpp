/* 
 * File:   program.cpp
 * Author: mkh
 * 
 * Created on 24 января 2023 г., 10:55
 */

#include "program.h"
#include "../../../share/utils.h"
#include <iostream>


program_error::program_error( const std::string &what )
: std::runtime_error( what )
{
}

namespace
{
bool block_variable( GLuint id, GLuint idx, std::pair< std::string, GLint > *rc )
{
    GLenum const properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };
    GLint results[4];
    glGetProgramResourceiv( id, GL_UNIFORM, idx, 4, properties, 4, nullptr, results );
    std::vector< char > name( results[0] + 1, ' ' );
    glGetProgramResourceName( id, GL_UNIFORM, idx, results[0] + 1, nullptr, name.data() );
    
    *rc = std::make_pair( std::string(name.data(), name.size()), results[2] );
    return results[3] != -1; 
}
}  // namespace

program::program( char const *filter )
: id_( glCreateProgram() )
{
    try
    {
        utils::read_directory( utils::config()["shaders"], filter, [this]( const std::string &name ){ f_emplace_shader( name ); } );
    }
    catch( const std::runtime_error & err )
    {
        glDeleteProgram( id_ );
        throw;
    }
    f_link();
}

program::~program()
{
    glDeleteProgram( id_ );
}

void program::f_emplace_shader( const std::string & path )
{
    shaders_.emplace_back( new shader( path ) );
    glAttachShader( id_, *shaders_.back() );
}

void program::f_link()
{
    if( shaders_.empty() )
    {
        throw program_error( "no shaders" );
    }
    glLinkProgram( id_ );
    f_check();
    
    GLint linked;
    glGetProgramiv( id_, GL_LINK_STATUS, &linked );
    if( linked != 1 )
    {
        f_throw_error();
    }
    shaders_.clear();
    f_get_attributes();
    f_get_uniforms();
    f_get_uniform_blocks();
}

GLuint program::attribute_index( const std::string &name ) const
{
    auto attr = attributes_.find( name );
    if( attr == attributes_.end() )
        throw program_error( std::string("attribute '" + name + "' not found") );
    return (*attr).second.first;
}

GLuint program::uniform_index( const std::string &name ) const
{
    auto attr = uniforms_.find( name );
    if( attr == uniforms_.end() )
        throw program_error( std::string("uniform '" + name + "' not found") );
    return (*attr).second;
}

uniformblock &program::uniform_block( std::string const &name ) {
    auto blk = uniform_blocks_.find( name );
    if( blk == uniform_blocks_.end() )
        throw program_error( std::string("uniform block '" + name + "' not found") );
    return *(*blk).second;
}

void program::f_check() {
    int glErr = glGetError();
    std::string what;
    while( glErr != GL_NO_ERROR ) {
        what.append( std::to_string( glErr ) + " ");
        glErr = glGetError();
    }
    if( !what.empty() )
        throw program_error( what );
}

void program::f_throw_error() {
    int len = 0;
    glGetProgramiv( id_, GL_INFO_LOG_LENGTH, &len );
    if( len > 0 ) {
        int chWrittn = 0;
        std::vector< char > log( len, 0 );
        glGetProgramInfoLog( id_, len, &chWrittn, log.data() );
        throw program_error( log.data() );
    }
}

void program::f_get_attributes() {
    GLint num;
    glGetProgramInterfaceiv( id_, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &num );
    GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };
    for( int i(0); i < num; ++i ) {
        GLint results[3];
        glGetProgramResourceiv( id_, GL_PROGRAM_INPUT, i, 3, properties, 3, nullptr, results );
        std::vector< char > name( results[0] + 1, ' ' );
        glGetProgramResourceName( id_, GL_PROGRAM_INPUT, i, results[0] + 1, nullptr, name.data() );
        attributes_.emplace( std::string(name.data()), std::make_pair( results[2], results[1] ) );
    }
    std::cerr << "attributes: \n";
    for( auto a : attributes_ )
        std::cerr << "\t'" << a.first << "': index=" << a.second.first << " type=" << a.second.second << std::endl;
}

void program::f_get_uniforms() {
    GLint num;
    glGetProgramInterfaceiv( id_, GL_UNIFORM, GL_ACTIVE_RESOURCES, &num );
    for( int i(0); i < num; ++i ) {
        std::pair< std::string, GLint > rc;
        if( !block_variable( id_, i, &rc ) )
        {
            uniforms_.emplace( rc.first, rc.second );
        }
    }
    std::cerr << "uniforms: \n";
    for( auto a : uniforms_ )
        std::cerr << "\t'" << a.first << "': index=" << a.second << std::endl;
}

void program::f_get_uniform_blocks() {
    GLint num;
    glGetProgramInterfaceiv( id_, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &num );
    GLenum props[] = { GL_NAME_LENGTH, GL_NUM_ACTIVE_VARIABLES };
    for( int i(0); i < num; ++i ) {
        GLint results[2];
        glGetProgramResourceiv( id_, GL_UNIFORM_BLOCK, i, 2, props, 2, nullptr, results );
        std::vector< char > v( results[0] + 1, ' ' );
        glGetProgramResourceName( id_, GL_UNIFORM_BLOCK, i, results[0] + 1, nullptr, v.data() );
        while( v.back() == 0 || v.back() == ' ' )
        {
            v.pop_back();
        }
	std::string name(v.data(), v.size());
        uniform_blocks_.emplace( name, new uniformblock( id_, name.data() ) );
        if( results[1] )
        {
            const GLenum var_props[1] = { GL_ACTIVE_VARIABLES };
            std::vector< GLint > vars( results[1] );
            glGetProgramResourceiv( id_, GL_UNIFORM_BLOCK, i, 1, var_props, results[1], nullptr, vars.data() );
            for( int v(0); v < results[1]; ++v )
            {
                std::pair< std::string, GLint > rc;
                if( block_variable( id_, vars[v], &rc ) )
                {
                    uniform_blocks_[name]->emplace_variable( rc.first, rc.second );
                }
            }
        }
    }
    std::cerr << "Uniform blocks: \n";
    for( auto a : uniform_blocks_ )
    {
        std::cerr << "\t'" << a.first << "' {" << std::endl;
        for( auto var : *a.second )
	    std::cerr << "\t    " << var.name() << std::endl;
	std::cerr << "\t}\n";
    }
}
