#ifndef PLVEXCEPTIONS_H
#define PLVEXCEPTIONS_H

#include <stdexcept>

namespace plv
{
    class PipelineException : public std::runtime_error
    {
    public:
        PipelineException( const std::string& str ) : std::runtime_error( str ) {}
    };

    class IllegalAccessException : public std::runtime_error
    {
    public:
        IllegalAccessException( const std::string& str ) : std::runtime_error( str ) {}
    };

   class IllegalArgumentException : public std::runtime_error
    {
    public:
        IllegalArgumentException( const std::string& str ) : std::runtime_error( str ) {}
    };

    class ElementCreationException : public std::runtime_error
    {
    public:
        ElementCreationException(std::string msg)
            : std::runtime_error(msg) {}
    };

    class InstantiationException : public std::runtime_error
    {
    public:
        InstantiationException(std::string msg)
            : std::runtime_error(msg) {}
    };

    class IncompatibleTypeException : public std::runtime_error
    {
    public:
        IncompatibleTypeException(std::string msg)
            : std::runtime_error(msg) {}
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
