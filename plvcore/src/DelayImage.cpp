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

#include "DelayImage.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

DelayImage::DelayImage():
        m_steps(5)
{
    m_inputPin = createInputPin<OpenCVImage>( INPUT_PIN_NAME, this, IInputPin::REQUIRED );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );

}

DelayImage::~DelayImage()
{
}

void DelayImage::init() throw (PipelineException)
{
    clearBuffer();
}
void DelayImage::start() throw (PipelineException)
{
    clearBuffer();
}
bool DelayImage::isReadyForProcessing() const
{
    return m_inputPin->hasData();
}

void DelayImage::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);


    RefPtr<OpenCVImage> imgIn = m_inputPin->get();

    //always a imgBuf that will be stored in the buffer
    RefPtr<OpenCVImage> imgBuf = OpenCVImageFactory::instance()->get(
            imgIn->getWidth(), imgIn->getHeight(), imgIn->getDepth(), imgIn->getNumChannels() );

    //always some output image: if not enough delayed frames, take current oldest frames
    RefPtr<OpenCVImage> imgOut = OpenCVImageFactory::instance()->get(
            imgIn->getWidth(), imgIn->getHeight(), imgIn->getDepth(), imgIn->getNumChannels() );

    // open input image for reading
    const IplImage* iplImgIn = imgIn->getImage();

    // open buffer image for writing
    IplImage* iplImgBuf = imgBuf->getImageForWriting();
    // open output image for writing
    IplImage* iplImgOut = imgOut->getImageForWriting();

    //copy input to buffer image
    cvCopy(iplImgIn, iplImgBuf);
    //determine index where we will store next image
    m_latestFrameIndex = (m_latestFrameIndex+1)%(MAX_STEPS+1);
    //check if we have an image there
    if (m_delayedImgs[m_latestFrameIndex] != 0)
    {   // if so, kill through refptr destruct. If I'm not wrong, this should put refcount to zero, and image will be destroyed.
        m_delayedImgs[m_latestFrameIndex]->~RefPtr();
    }
    //store the buffer image
    m_delayedImgs[m_latestFrameIndex] = new RefPtr<OpenCVImage>(imgBuf); //in a RefPtr, so we'll not lose the image when this process() ends :)
    //determine index of buffered image to show, and get image.
    //note that if not enough images, we use the oldest available
    int prevImgToShowIndex = m_latestFrameIndex;
    int imgToShowIndex = m_latestFrameIndex;
    const IplImage* iplImgToShow;
    int stepsTaken = 0;
    while (true)
    {
        if (m_delayedImgs[imgToShowIndex]==0)
        {   //not enough buffer; use latest
            iplImgToShow = (*m_delayedImgs[prevImgToShowIndex])->getImage();
            //qDebug() << "hello 1 on frame: " << imgToShowIndex << " after " << stepsTaken << " steps";
            break;
        }
        else if (stepsTaken == m_steps)
        {   //taken enough steps; use this image
            iplImgToShow = (*m_delayedImgs[imgToShowIndex])->getImage();
            //qDebug() << "hello 2 on frame: " << imgToShowIndex << " after " << stepsTaken << " steps";
            break;
        }
        else
        {   //keep looking
            //qDebug() << "hello 3 on frame: " << imgToShowIndex << " after " << stepsTaken << " steps";
            prevImgToShowIndex = imgToShowIndex;
            stepsTaken++;
            imgToShowIndex--;
            if (imgToShowIndex < 0) imgToShowIndex = MAX_STEPS;
        }
    }

    //copy the DelayImage to the output
    cvCopy(iplImgToShow,iplImgOut);


    // publish the new image
    m_outputPin->put( imgOut.getPtr() );

}

void DelayImage::clearBuffer()
{
    //FIXME: mutex with process!
    for (int i = 0; i <= MAX_STEPS; i++)
    {
        m_delayedImgs[i] = 0;
    }
    m_latestFrameIndex = 0;
}

void DelayImage::setSteps(int i)
{   //stgeps must be positive and maximum MAX_STEPS
    //FIXME: mutex with process!
    if (i < 0) i = 0;
    if (i > MAX_STEPS) i = MAX_STEPS;
    m_steps = i;
    emit(stepsChanged(m_steps));
}
