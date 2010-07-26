#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

namespace plvgui
{
    class FatalError : public std::runtime_error
    {
    public:
        FatalError( const std::string& str ) : std::runtime_error( str ) {}
    };

    class NonFatalException : public std::runtime_error
    {
    public:
        NonFatalException( const std::string& str ) : std::runtime_error( str ) {}
    };
}

#endif // EXCEPTIONS_H
