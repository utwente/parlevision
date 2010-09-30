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

#ifndef REFCOUNTED_H
#define REFCOUNTED_H

#include <QMutex>
#include <QMutexLocker>
#include <QDebug>

#include "plvglobal.h"

namespace plv
{

class PLVCORE_EXPORT RefCounted
{
protected:
    mutable int m_referenceCount;
    mutable QMutex m_refMutex;

public:
    /** Initializes reference count to 0 */
    RefCounted();

    /** Copy constructor of RefCounted objects initializes
      * the reference count of the copy to 0.
      */
    explicit RefCounted( const RefCounted& )
        : m_referenceCount( 0 )
    {
    }

    /** RefCounted objects should generally not explicitely be destructed.
      * However, the Qt meta object system requires the destructor to be
      * public.
      */
    virtual ~RefCounted();

    /** increases reference count by one */
    inline void inc() const
    {
        QMutexLocker lock( &m_refMutex );
        ++m_referenceCount;
    }

    /** decreases reference count by one. Deletes this object if
      * reference count becomes 0
      */
    inline void dec() const
    {
        m_refMutex.lock();
        --m_referenceCount;
        if( m_referenceCount < 1 )
        {
            m_refMutex.unlock();
            delete this;
            return;
        }
        m_refMutex.unlock();
    }

    /** @return number of references to this object */
    inline int getRefCount() const { return m_referenceCount; }
};

}

#endif // REFCOUNTED_H
