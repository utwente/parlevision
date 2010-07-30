#ifndef PLVEXCEPTIONS_H
#define PLVEXCEPTIONS_H

#include <stdexcept>
#include <QString>

namespace plv
{
    class PlvException : public std::runtime_error
    {
    public:
        PlvException( const QString& str ) : std::runtime_error( str.toStdString() ) {}
        PlvException( const std::string& str ) : std::runtime_error( str ) {}
    };

    class PipelineException : public PlvException
    {
    public:
        PipelineException( const std::string& str ) : PlvException( str ) {}
    };

    class IllegalAccessException : public PlvException
    {
    public:
        IllegalAccessException( const std::string& str ) : PlvException( str ) {}
    };

   class IllegalArgumentException : public PlvException
    {
    public:
        IllegalArgumentException( const std::string& str ) : PlvException( str ) {}
    };

    class ElementCreationException : public PlvException
    {
    public:
        ElementCreationException(std::string msg)
            : PlvException(msg) {}
    };

    class InstantiationException : public PlvException
    {
    public:
        InstantiationException(std::string msg)
            : PlvException(msg) {}
    };

    class IncompatibleTypeException : public PlvException
    {
    public:
        IncompatibleTypeException(std::string msg)
            : PlvException(msg) {}
    };

    class DuplicateConnectionException : public PlvException
    {
    public:
        DuplicateConnectionException(std::string msg)
            : PlvException(msg) {}
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
