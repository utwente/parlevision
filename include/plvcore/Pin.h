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

#ifndef PIN_H
#define PIN_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <assert.h>

#include "RefCounted.h"

namespace plv
{
    class PipelineElement;

    /** abstract base class of input and output pins */
    class PLVCORE_EXPORT Pin : public QObject, public RefCounted
    {
        Q_OBJECT

        /** Pins can access PipelineElements protected methods */
        friend class PipelineElement;

    public:

        Pin( const QString& name, PipelineElement* owner ) :
            m_id(-1),
            m_name( name ),
            m_pleOwner( owner )
        {
            assert( !m_name.isEmpty() );
            assert( m_pleOwner != 0 );
        }

        Pin( int id, const QString& name, PipelineElement* owner ) :
            m_id( id ),
            m_name( name ),
            m_pleOwner( owner )
        {
            assert( m_id != -1 );
            assert( !m_name.isEmpty() );
            assert( m_pleOwner != 0 );
        }

        inline const QString& getName() const { return m_name; }
        inline PipelineElement* getOwner() const { return m_pleOwner; }

        inline void setId(int id) { m_id = id; }
        inline int getId() const { return m_id; }

        /** @returns the QMetaType typeId of the data type this pin is initialized with */
        virtual int getTypeId() const = 0;

        /** @returns the name of the type this pin is initialized with */
        virtual QString getTypeName() const = 0;

        void setName(const QString& name);

    signals:
        void nameChanged(const QString& name);
        void newData( unsigned int serial, QVariant v );
        void error( QString msg );

    protected:
        /** id of this pin */
        int m_id;

        /** the name of this Pin e.g. "BlackAndWhite" */
        QString m_name;

        /** data received from or delivered to this pipeline element */
        PipelineElement* m_pleOwner;
    };
}

#endif // PIN_H
