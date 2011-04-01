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

#ifndef BLOBDETECTOR_H
#define BLOBDETECTOR_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>
#include <vector>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class Blob;

    class BlobDetector : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( BlobDetector )
        Q_CLASSINFO("author", "Richard")
        Q_CLASSINFO("name", "Blob Detector")
        Q_CLASSINFO("description", "Detects blobs" );

        //Q_PROPERTY( plv::Enum mode READ getMode WRITE setMode NOTIFY modeChanged )
        //Q_PROPERTY( plv::Enum method READ getMethod WRITE setMethod NOTIFY methodChanged )
        //Q_PROPERTY( double beta READ getBeta WRITE setBeta NOTIFY betaChanged )
        //Q_PROPERTY( double gamma READ getGamma WRITE setGamma NOTIFY gammaChanged )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR

    public:
        BlobDetector();
        virtual ~BlobDetector();

        plv::Enum getMethod() const;
        plv::Enum getMode() const;

    public slots:
        void setMode( plv::Enum mode );
        void setMethod( plv::Enum method );

    signals:
        void modeChanged( plv::Enum mode );
        void methodChanged( plv::Enum method );

    private:
        plv::CvMatDataInputPin* m_inputImage;
        plv::CvMatDataOutputPin* m_outputImage;

        plv::Enum m_mode; /* Contour retrieval modes */
        plv::Enum m_method; /* Contour approximation methods */

        QList<Blob*> m_blobs;
        unsigned int m_idCounter;
        unsigned int m_nrFramesToKeep;

        inline unsigned int getNewId() { return ++m_idCounter; }

        QList<Blob*> matchBlobs( QList<Blob*>& newBlobs, const QList<Blob*>& oldBlobs);
    };
}
#endif // BLOBDETECTOR_H
