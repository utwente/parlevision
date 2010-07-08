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

//    class DataFormatException : public PipelineException
//    {
//    public:
//        RefPtr<Pin> pin;
//        RefPtr<PipelineElement> source;
//        DataFormatException( const std::string& msg, PipelineElement* source, Pin* p)
//            : PipelineException( msg ),
//            pin(p),
//            source(source)
//        {};
//        ~DataFormatException() throw();
//    };
}


#endif // PLVEXCEPTIONS_H
