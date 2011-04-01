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

#ifndef BLOBTRACKER_H
#define BLOBTRACKER_H

#include <plvcore/PipelineProcessor.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class BlobTracker : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( BlobTracker )
        Q_CLASSINFO("author", "Richard")
        Q_CLASSINFO("name", "Blob Tracker")
        Q_CLASSINFO("description", "Tracks blobs" );

        //Q_PROPERTY( double alpha READ getAlpha WRITE setAlpha NOTIFY alphaChanged )
        //Q_PROPERTY( double beta READ getBeta WRITE setBeta NOTIFY betaChanged )
        //Q_PROPERTY( double gamma READ getGamma WRITE setGamma NOTIFY gammaChanged )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR

    public:
        BlobTracker();
        virtual ~BlobTracker();

    private:
        plv::CvMatDataInputPin* m_inputImage;
    };
}
#endif // BLOBTRACKER_H
