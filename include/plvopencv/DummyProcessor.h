/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvopencv module of ParleVision.
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

#ifndef DUMMYPROCESSOR_H
#define DUMMYPROCESSOR_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Types.h>
#include <plvcore/Enum.h>

namespace plv
{
    class OpenCVImage;
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class DummyProcessor : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( DummyProcessor )
        Q_CLASSINFO("author", "Michel & Richard")
        Q_CLASSINFO("name", "Dummy")
        Q_CLASSINFO("description", "A simple processor to demonstrate how to implement your own processors. "
                    "It doesn't do much, but flip your images.");

        Q_PROPERTY( int someInt READ getSomeInt WRITE setSomeInt NOTIFY someIntChanged  )
        Q_PROPERTY( double someDouble READ getSomeDouble WRITE setSomeDouble NOTIFY someDoubleChanged  )
        Q_PROPERTY( bool someBool READ getSomeBool WRITE setSomeBool NOTIFY someBoolChanged  )
        Q_PROPERTY( QString someString READ getSomeString WRITE setSomeString NOTIFY someStringChanged )
        Q_PROPERTY( int someVarWithNr1 READ getSomeVarWithNr1 WRITE setSomeVarWithNr1 NOTIFY someVarWithNr1Changed )
        Q_PROPERTY( int someVarWithNr2 READ getSomeVarWithNr2 WRITE setSomeVarWithNr2 NOTIFY someVarWithNr2Changed )
        Q_PROPERTY( plv::Enum customEnum READ getCustomEnum WRITE setCustomEnum NOTIFY customEnumChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        DummyProcessor();
        virtual ~DummyProcessor();

        /** propery methods */
        int getSomeInt() { return m_someInt; }
        double getSomeDouble() { return m_someDouble; }
        bool getSomeBool() { return m_someBool; }
        QString getSomeString() { return m_someString; }
        plv::Enum getCustomEnum() const { return m_customEnum; }

        int getSomeVarWithNr1() { return m_someVarWithNr1; }
        int getSomeVarWithNr2() { return m_someVarWithNr2; }

    signals:
        void someIntChanged(int newValue);
        void someDoubleChanged(double newValue);
        void someBoolChanged(bool newValue);
        void someStringChanged(QString newValue);
        void someVarWithNr1Changed(int var);
        void someVarWithNr2Changed(int var);
        void customEnumChanged( plv::Enum p );

    public slots:
        void setSomeInt(int i) {m_someInt = i; emit(someIntChanged(i));}
        void setSomeDouble(double d) {m_someDouble = d; emit(someDoubleChanged(d));}
        void setSomeBool(bool b) {m_someBool = b; emit(someBoolChanged(b));}
        void setSomeString(QString s) {m_someString = s; emit(someStringChanged(s));}
        void setSomeVarWithNr1(int var) { m_someVarWithNr1 = var; emit( someVarWithNr1Changed(var) ); }
        void setSomeVarWithNr2(int var) { m_someVarWithNr2 = var; emit( someVarWithNr2Changed(var) ); }
        void setCustomEnum( plv::Enum c ) { m_customEnum = c; emit( customEnumChanged( c ) ); }

    private:
        plv::CvMatDataInputPin*  m_inputPin;
        plv::CvMatDataInputPin*  m_inputPinOptional;
        plv::CvMatDataOutputPin* m_outputPin;

        int m_someInt;
        double m_someDouble;
        bool m_someBool;
        QString m_someString;
        int m_someVarWithNr1;
        int m_someVarWithNr2;
        plv::Enum m_customEnum;
    };

}

#endif // DUMMYPROCESSOR_H
