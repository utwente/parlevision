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

#ifndef PLVBLOBTRACK_BLOBTRACKER_H
#define PLVBLOBTRACK_BLOBTRACKER_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/InputPin.h>
#include <QStringList>

#include "Blob.h"
#include "BlobTrack.h"

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvblobtracker
{
    class BlobTracker : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( BlobTracker )
        Q_CLASSINFO("author", "Richard")
        Q_CLASSINFO("name", "Blob Tracker")
        Q_CLASSINFO("description", "Tracks blobs" )

        //Q_PROPERTY( double alpha READ getAlpha WRITE setAlpha NOTIFY alphaChanged )
        //Q_PROPERTY( double beta READ getBeta WRITE setBeta NOTIFY betaChanged )
        //Q_PROPERTY( double gamma READ getGamma WRITE setGamma NOTIFY gammaChanged )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR

    public:
        BlobTracker();
        virtual ~BlobTracker();

        // overloaded function from PipelineElement
        bool init();
        bool deinit() throw();
        bool start();
        bool stop();

    private:
        plv::CvMatDataInputPin* m_inputImage;
        plv::InputPin< QList<plvblobtracker::Blob> >* m_inputBlobs;
        plv::CvMatDataOutputPin* m_outputImage;
        QList<BlobTrack> m_blobTracks;

        void matchBlobs(QList<Blob>& newBlobs, QList<BlobTrack>& blobTracks);
        unsigned int m_idCounter;
        inline unsigned int getNewId() { return ++m_idCounter; }


    };
}
#endif // BLOBTRACKER_H
