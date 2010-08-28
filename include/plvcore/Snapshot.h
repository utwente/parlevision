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

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {

    class Pipeline;
    class OpenCVImage;

    class Snapshot : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Dennis")
        Q_CLASSINFO("name", "Snapshot")
        Q_CLASSINFO("description", "On request, make a new snapshot of the input pin. On the output pin, always present the latest snapshot. Always enforces a snapshot on the first frame.");

        Q_PROPERTY( bool makeSnapshot READ getMakeSnapshot WRITE setMakeSnapshot NOTIFY makeSnapshotChanged  )


    public:
        Snapshot();
        ~Snapshot();

        virtual void init() throw (PipelineException);
        /** every start, a new snapshot must be enforced */
        virtual void start() throw (PipelineException);
        virtual void stop() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        bool getMakeSnapshot() { return m_makeSnapshot; }

    signals:
        void makeSnapshotChanged(bool newValue);

    public slots:
        void setMakeSnapshot(bool b) {m_makeSnapshot = b; emit(makeSnapshotChanged(b));}

    private:
        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;
        RefPtr<OpenCVImage> m_imgSnapshot;

        bool m_makeSnapshot;

    };

}

#endif // SNAPSHOT_H
