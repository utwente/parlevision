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

#include "plvglobal.h"

namespace plv
{
    class PLVCORE_EXPORT Exception : public std::runtime_error
    {

    public:
        Exception(const QString& str) : std::runtime_error(str.toStdString()){}
        virtual ~Exception() throw() {}
    };

    class PLVCORE_EXPORT RuntimeError : public Exception
    {
    protected:
        QString m_fileName;
        int m_lineNumber;
    public:
        RuntimeError(const QString& str, QString fileName, int lineNumber)
            : Exception(str),
            m_fileName(fileName),
            m_lineNumber(lineNumber)
        {
        }
        virtual ~RuntimeError() throw() {}

        inline QString getFileName() const { return m_fileName; }
        inline int getLineNumber() const { return m_lineNumber; }
    };

    class PLVCORE_EXPORT IllegalArgumentException : public Exception
    {
    public:
        IllegalArgumentException(const QString& str) : Exception(str) {}
        virtual ~IllegalArgumentException() throw() {}
    };

//    class PLVCORE_EXPORT PlvFatalException : public PlvRuntimeException
//    {
//    public:
//        PlvFatalException(const QString& str, QString fileName, int lineNumber )
//            : PlvRuntimeException( str, fileName, lineNumber ) {}
//        virtual ~PlvFatalException() throw() {}
//    };
//    class PLVCORE_EXPORT PlvInitialisationException : public PlvException
//    {
//    public:
//        PlvInitialisationException(const QString& str) : PlvException(str) {}
//        virtual ~PlvInitialisationException() throw() {}
//    };
//    class PLVCORE_EXPORT IllegalAccessException : public PlvException
//    {
//    public:
//        IllegalAccessException(const QString& str, QString fileName, int lineNumber )
//            : PlvException( str, fileName, lineNumber )
//        {
//        }
//        virtual ~IllegalAccessException() throw() {}
//    };

//    class PLVCORE_EXPORT ElementCreationException : public PlvException
//    {
//    public:
//        ElementCreationException(const QString& str, QString fileName, int lineNumber )
//            : PlvException( str, fileName, lineNumber )
//        {
//        }
//        virtual ~ElementCreationException() throw() {}
//    };

//    class PLVCORE_EXPORT InstantiationException : public PlvException
//    {
//    public:
//        InstantiationException(const QString& str, QString fileName, int lineNumber )
//            : PlvException( str, fileName, lineNumber )
//        {
//        }
//        virtual ~InstantiationException() throw() {}
//    };


}


#endif // PLVEXCEPTIONS_H
