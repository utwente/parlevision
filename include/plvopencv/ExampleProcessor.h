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

#ifndef EXAMPLEPROCESSOR_H
#define EXAMPLEPROCESSOR_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class ExampleProcessor : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( ExampleProcessor )
        Q_CLASSINFO("author", "Michel & Richard")
        Q_CLASSINFO("name", "Example")
        Q_CLASSINFO("description", "A simple processor to demonstrate how to implement your own processors. "
                    "It doesn't do much, but flip your images.");

        Q_PROPERTY( int someInt READ getSomeInt WRITE setSomeInt NOTIFY someIntChanged  )
        Q_PROPERTY( double someDouble READ getSomeDouble WRITE setSomeDouble NOTIFY someDoubleChanged  )
        Q_PROPERTY( bool someBool READ getSomeBool WRITE setSomeBool NOTIFY someBoolChanged  )
        Q_PROPERTY( QString someString READ getSomeString WRITE setSomeString NOTIFY someStringChanged )
        Q_PROPERTY( plv::Enum customEnum READ getCustomEnum WRITE setCustomEnum NOTIFY customEnumChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        ExampleProcessor();
        virtual ~ExampleProcessor();

        /** propery methods getters */
        int getSomeInt();
        double getSomeDouble();
        bool getSomeBool();
        QString getSomeString();
        plv::Enum getCustomEnum() const;

    public slots:
        void setSomeInt(int i);
        void setSomeDouble(double d);
        void setSomeBool(bool b);
        void setSomeString(QString s);
        void setCustomEnum(plv::Enum c);

    signals:
        void someIntChanged(int i);
        void someDoubleChanged(double d);
        void someBoolChanged(bool b);
        void someStringChanged(QString a);
        void customEnumChanged( plv::Enum p );

    public:
        void updateSomeInt(int i) { setSomeInt(i); someIntChanged(i); }
        void updateSomeDouble(double d) { setSomeDouble(d); someDoubleChanged(d); }
        void updateSomeBool(bool b) { setSomeBool(b); someBoolChanged(b); }
        void updateSomeString(QString s) { setSomeString(s); someStringChanged(s); }
        void updateCustomEnum(plv::Enum c) { setCustomEnum(c); customEnumChanged(c); }

    private:
        plv::CvMatDataInputPin*  m_inputPin;
        plv::CvMatDataInputPin*  m_inputPinOptional;
        plv::CvMatDataOutputPin* m_outputPin;

        int m_someInt;
        double m_someDouble;
        bool m_someBool;
        QString m_someString;
        plv::Enum m_customEnum;
    };

}

#endif // EXAMPLEPROCESSOR_H
