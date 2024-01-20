/* 
 * File:   baseprotocol.cpp
 * Author: mkh
 * 
 * Created on 25 января 2023 г., 14:59
 */

#include "baseprotocol.h"
#include <cstring>
#include <cerrno>

protocol_error::protocol_error( const std::string &what )
: std::runtime_error( what + std::string(" failed: ") + std::string(strerror( errno )) )
{}

baseprotocol::baseprotocol( int sock )
: fd_( sock )
{}

baseprotocol::~baseprotocol()
{}
