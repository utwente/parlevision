/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

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
