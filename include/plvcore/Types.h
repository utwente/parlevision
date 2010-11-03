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

#ifndef TYPES_H
#define TYPES_H

#include <QMap>
#include <QStringList>
#include <QStringBuilder>
#include <QMetaType>
#include <QVariant>

#include "RefPtr.h"
#include "assert.h"
#include "plvglobal.h"

namespace plv 
{
    /** Base class for data resources.
      * Data resources are not allowed to be deleted explicitly since they can be
      * shared. Explicit deletion could cause a crash. Reference counting
      * is used to let data resources delete themselves.
      */
    class PLVCORE_EXPORT Data : public RefCounted
    {
    protected:
        /** serial number, used for synchronisation */
        unsigned int m_serial;

        /** mutable flag used for determining if data resource can be written to */
        bool m_mutable;

    public:
        Data(unsigned int serial = 0) : m_serial(serial), m_mutable(true) {}

        /** Copy constructor needs to be implemented by super classes
          * to allow the copying of a data resources when the Pin
          * connection type is set to copy which can be faster with
          * simple types.
          */
        Data(const Data& other): m_serial(other.m_serial), m_mutable(true) {}

        /** Destructor, should not be called explicitly because of reference counting */
        virtual ~Data() {}

        /** makes this data unit mutable again. Internal framework method.
          * Should normally not be called by client code
          */
        inline void makeMutable()
        {
            QMutexLocker( &this->m_refMutex );
            m_mutable = true;
        }

        /** makes this data element immutable. This is called by the framework when this element
          * is read only shared in multiple threads
          */
        inline void makeImmutable()
        {
            QMutexLocker( &this->m_refMutex );
            m_mutable = false;
        }

        inline bool isMutable() const
        {
            QMutexLocker( &this->m_refMutex );
            return m_mutable;
        }

        inline unsigned int getSerial() const
        {
            QMutexLocker( &this->m_refMutex );
            return m_serial;
        }

        inline void setSerial( unsigned int serial )
        {
            QMutexLocker( &this->m_refMutex );
            m_serial = serial;
        }

        /** used to signal a NULL entry, generally there will be no
          * data items sent with serial number 0. Null entries are ignored
          * by viewers but used to synchronize the system. This is done
          * automatically. Producers should generally never produce a Data item
          * with serial number 0.
          */
        inline bool isNull() const
        {
            QMutexLocker( &this->m_refMutex );
            return m_serial == 0;
        }
    };

    /** Template class to make the implementation of primitive data types
      * trivial. Do not use for pointer types!
      */
    template< class T >
    class PLVCORE_EXPORT PrimitiveData : public Data
    {
    protected:
        mutable QMutex m_sdMutex;
        T m_value;

    public:
        PrimitiveData( T value ) : m_value( value ) {}

        inline T getValue() const
        {
            QMutexLocker lock( &m_sdMutex );
            return m_value;
        }

        inline T setValue(T value)
        {
            QMutexLocker lock( &m_sdMutex );
            m_value = value;
        }
    };
}

/** primitive types */
typedef plv::PrimitiveData<bool> PlvBoolean;
typedef plv::PrimitiveData<int> PlvInteger;
typedef plv::PrimitiveData<float> PlvFloat;
typedef plv::PrimitiveData<double> PlvDouble;
typedef plv::PrimitiveData<QString> PlvString;

/** Declare as Qt Metatype so we can pass RefPtr<Data> along with signals and slots */
Q_DECLARE_METATYPE( plv::RefPtr<plv::Data> )
Q_DECLARE_METATYPE( plv::RefPtr<PlvBoolean> )
Q_DECLARE_METATYPE( plv::RefPtr<PlvInteger> )
Q_DECLARE_METATYPE( plv::RefPtr<PlvFloat> )
Q_DECLARE_METATYPE( plv::RefPtr<PlvDouble> )
Q_DECLARE_METATYPE( plv::RefPtr<PlvString> )

#endif // TYPES_H
