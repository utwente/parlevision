#ifndef PLVEXCEPTIONS_H
#define PLVEXCEPTIONS_H

#include <stdexcept>

namespace plv
{

    class PipelineException : public std::runtime_error
    {
    public:
        PipelineException( const std::string& str ) : std::runtime_error( str ) {};
    };

    class IllegalAccessException : public std::runtime_error
    {
    public:
        IllegalAccessException( const std::string& str ) : std::runtime_error( str ) {};
    };

    class IllegalArgumentException : public std::runtime_error
    {
    public:
        IllegalArgumentException( const std::string& str ) : std::runtime_error( str ) {};
    };

    class ElementCreationException : public std::runtime_error
    {
    public:
        ElementCreationException(std::string msg)
            : std::runtime_error(msg) {}
    };

}


#endif // PLVEXCEPTIONS_H
