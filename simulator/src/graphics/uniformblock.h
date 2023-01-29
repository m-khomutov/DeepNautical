/* 
 * File:   uniformblock.h
 * Author: mkh
 *
 * Created on 29 января 2023 г., 11:52
 */

#ifndef UNIFORMBLOCK_H
#define UNIFORMBLOCK_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include <string>
#include <stdexcept>


class uniformblock_error: public std::runtime_error {
public:
    uniformblock_error(const std::string & what);
};

class uniformblock {
public:
    class variable
    {
    public:
        variable( const GLchar *name, GLint index )
        : name_( name )
        , index_( index ) 
        {
        }
        operator const GLfloat *() const {
            return value_.data();
        }
        std::string const &name() const
        {
            return name_;
        }
        GLint index() const
        {
            return index_;    
        }
        GLuint size() const
        {
            return value_.size() *sizeof(GLfloat);
        }

        variable &operator =( GLfloat value )
        {
            value_.resize( 1 );
            value_[0] = value;
            return *this;    
        }
        variable &operator =( glm::vec4 value )
        {
            value_.resize( 4 );
            for (GLuint i(0); i < 4; ++i )
            {
                value_[i] = value[i];
            }
            return *this;    
        }

    private:
        std::string name_;
        GLint index_;
        std::vector< GLfloat> value_;
    };

    uniformblock( GLuint program_id, const GLchar *name );
    uniformblock(const uniformblock& orig) = delete;
    uniformblock &operator =(const uniformblock& orig) = delete;
    ~uniformblock();

    void emplace_variable( std::string const &name, GLint index );
    void copy();
    variable &operator []( GLchar const *name );
    
    std::vector< variable >::iterator begin()
    {
        return variables_.begin();
    }
    std::vector< variable >::iterator end()
    {
        return variables_.end();
    }
    
private:
    GLuint program_id_;
    GLuint id_;
    std::string name_;
    std::vector< variable > variables_;
    std::vector< GLubyte > buffer_;
    GLuint ubo_;

};

#endif /* UNIFORMBLOCK_H */
