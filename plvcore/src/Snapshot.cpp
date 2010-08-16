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

#include <QDebug>

#include "Snapshot.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

Snapshot::Snapshot() :
        m_makeSnapshot(true)
{
    m_inputPin = createInputPin<OpenCVImage>( INPUT_PIN_NAME, this, IInputPin::INPUT_REQUIRED );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
}

Snapshot::~Snapshot()
{
}

void Snapshot::init() throw (PipelineException)
{
    m_imgSnapshot = RefPtr<OpenCVImage>();
}
void Snapshot::start() throw (PipelineException)
{
    m_imgSnapshot = RefPtr<OpenCVImage>();
}
bool Snapshot::isReadyForProcessing() const
{
    return m_inputPin->hasData();
}

void Snapshot::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);
    RefPtr<OpenCVImage> imgIn = m_inputPin->get();

    //enforce snapshot on first frame, when we don't have a snapshot yet
    if ((!m_makeSnapshot) && m_imgSnapshot.isNull())setMakeSnapshot(true);

    RefPtr<OpenCVImage> imgOut = OpenCVImageFactory::instance()->get(
            imgIn->getWidth(), imgIn->getHeight(), imgIn->getDepth(), imgIn->getNumChannels() );

    // open for reading
    const IplImage* iplImgIn = imgIn->getImage();

    // open image for writing
    IplImage* iplImgOut = imgOut->getImageForWriting();

    //if we did not yet have a snapshot, make an empty one
    if (m_imgSnapshot.isNull())
    {   //only needed when no snapshot was yet present
        m_imgSnapshot = OpenCVImageFactory::instance()->get(
                imgIn->getWidth(), imgIn->getHeight(), imgIn->getDepth(), imgIn->getNumChannels() );

    }

    //get the stored snapshot image, to copy it to the output
    IplImage* iplImgSnapshot = m_imgSnapshot->getImageForWriting();

    //make snapshot if requested
    if (m_makeSnapshot)
    {
        m_imgSnapshot = OpenCVImageFactory::instance()->get(
                imgIn->getWidth(), imgIn->getHeight(), imgIn->getDepth(), imgIn->getNumChannels() );
        // open the new snapshot image for writing
        iplImgSnapshot = m_imgSnapshot->getImageForWriting();
        //put input data into new snapshot image
        cvCopy(iplImgIn,iplImgSnapshot);
        //we made the snapshot, so set property to false again
        setMakeSnapshot(false);
    }

    //copy the snapshot to the output
    cvCopy(iplImgSnapshot,iplImgOut);

    // publish the new image
    m_outputPin->put( imgOut.getPtr() );

}

