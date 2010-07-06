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

}


#endif // PLVEXCEPTIONS_H
