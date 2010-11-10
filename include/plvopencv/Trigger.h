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
#include <plvcore/Types.h>

namespace plv
{
    class OpenCVImage;
}

namespace plvopencv
{
    /**
     * For every image this proccessor recieves it sends out a boolean signal.
     * This signal is of the same value as the processors activation property.
     * It can be used by proccessors that use an activation signal in their
     * processing. And very useful if a number of processors have to be acti-
     * vated on the same image. However this does require that no imagedata
     * gets lost in the pipeline. If image data gets lost synchronization pro-
     * blems will occur, because more triggers than images will enter the pro-
     * cessor.
     *
     * If the continuous property holds true the activation won't be set false
     * after each processed image. This allows some true activations to be sent
     * for a series of images. And requires the user to deactivate the trigger
     * manually.
     */
    class Trigger : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( Trigger )
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "Trigger")
        Q_CLASSINFO("description", "A processor that passes on a boolean value for every received image. "
                        "The boolean value can be used by other processors that accept it as an activation trigger. "
                        "The trigger is either continuous or one time only.");

        Q_PROPERTY( bool activate READ getActivate WRITE setActivate NOTIFY activateChanged  )
        Q_PROPERTY( bool continuous READ getContinuous WRITE setContinuous NOTIFY continuousChanged  )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        /** Constructor/Destructor */
        Trigger();
        virtual ~Trigger();

        /** propery methods */
        bool getActivate(){ return m_activate; }
        bool getContinuous(){ return m_continuous; }

    signals:
        void activateChanged (bool newValue);
        void continuousChanged (bool newValue);

    public slots:
        void setActivate(bool b) {m_activate = b; emit(activateChanged(b));}
        void setContinuous(bool b) {m_continuous = b; emit(continuousChanged(b));}

    private:
        plv::InputPin<plv::Data>* m_inputPin;
        plv::OutputPin<PlvBoolean>* m_outputPin;

        bool m_activate;   /** Determines if an activation (true) trigger has to be send. Mostly false. */
        bool m_continuous; /** Determines if the activation trigger automatically has to be switched off (false). */

    };

}

#endif // TRIGGER_H
