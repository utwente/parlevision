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

#ifndef TRIGGER_H
#define TRIGGER_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Pin.h>

namespace plvopencv
{
    class Trigger : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( Trigger )
        Q_CLASSINFO("author", "Richard Loos, Niek Hoeijmakers")
        Q_CLASSINFO("name", "Trigger")
        Q_CLASSINFO("description", "A processor that sends a boolean value when the input count has reached a number. "
                        "The boolean value can be used by other processors that accept it as an activation trigger. "
                        "The trigger is either continuous or one time only.");

        Q_PROPERTY( bool activate READ getActivate WRITE setActivate NOTIFY activateChanged  )
        Q_PROPERTY( bool continuous READ getContinuous WRITE setContinuous NOTIFY continuousChanged  )
        Q_PROPERTY( int count READ getCount WRITE setCount NOTIFY countChanged  )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_PROCESSOR

    public:
        /** Constructor/Destructor */
        Trigger();
        virtual ~Trigger();

        /** propery methods */
        bool getActivate() const;
        bool getContinuous() const;
        int getCount() const;

    signals:
        void activateChanged (bool newValue);
        void continuousChanged (bool newValue);
        void countChanged(int c);

    public slots:
        void setActivate(bool b);
        void setContinuous(bool b);
        void setCount(int i);

    private:
        void updateActivate(bool b){ setActivate(b); activateChanged(b); }
        void updateContinuous(bool b){ setContinuous(b); continuousChanged(b); }

        plv::DynamicInputPin* m_inputPin;
        plv::InputPin<int>* m_inputCount;
        plv::OutputPin<bool>*    m_outputPin;

        bool m_activate;   /** Determines if an activation (true) trigger has to be send. Mostly false. */
        bool m_continuous; /** Determines if the activation trigger automatically has to be switched off (false). */
        int m_count; /** the number of inputs received before it triggers */
        int m_received; /** the number of received inputs so far */
    };

}

#endif // TRIGGER_H
