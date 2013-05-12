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

#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/CvMatData.h>
#include <plvcore/InputPin.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class Snapshot : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( Snapshot )
        Q_CLASSINFO("author", "Dennis")
        Q_CLASSINFO("name", "Snapshot")
        Q_CLASSINFO("description", "On request, make a new snapshot of the "
                    "input pin. On the output pin, always present the latest "
                    "snapshot. Always enforces a snapshot on the first frame.")

        Q_PROPERTY( bool makeSnapshot READ getMakeSnapshot WRITE setMakeSnapshot NOTIFY makeSnapshotChanged  )
        Q_PROPERTY( bool keepSending READ getKeepSending WRITE setKeepSending NOTIFY keepSendingChanged  )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR

    public:
        Snapshot();
        virtual ~Snapshot();

        /** propery methods */
        bool getMakeSnapshot() const;
        bool getKeepSending() const;

    signals:
        void makeSnapshotChanged(bool b);
        void keepSendingChanged(bool b);

    public slots:
        void setMakeSnapshot(bool b);
        void setKeepSending(bool b);

    private:
        plv::CvMatDataInputPin* m_inputPin;
        plv::InputPin<bool>* m_inputTrigger;
        plv::CvMatDataOutputPin* m_outputPin;
        plv::CvMatData m_snapshot;
        bool m_makeSnapshot;
        bool m_keepSending;
    };

}

#endif // SNAPSHOT_H
