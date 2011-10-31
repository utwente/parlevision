/**
  * Copyright (C)2011 by Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvtest plugin of ParleVision.
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

#include "BlobProducer.h"

#include <QDebug>
#include <plvcore/Pin.h>

using namespace plv;

BlobProducer::BlobProducer() :
    m_width(640),
    m_height(480),
    m_maxStep(10),
    m_numBlobs(10)
{
    m_outputPin = createCvMatDataOutputPin( "output", this );

    m_outputPin->addSupportedChannels(1);
    m_outputPin->addSupportedDepth(CV_8U);
}

BlobProducer::~BlobProducer()
{
}

bool BlobProducer::init()
{
    m_positions = QVector<cv::Point>(m_numBlobs, cv::Point(0,0));
    m_targets   = QVector<cv::Point>(m_numBlobs, cv::Point(m_width/2, m_height/2));
    m_factor = (m_maxStep / (double)RAND_MAX);

    return true;
}

bool BlobProducer::readyToProduce() const
{
    return true;
}

bool BlobProducer::produce()
{
    CvMatData out = CvMatData::create(640,480,CV_8UC1);
    cv::Mat& img = out;
    img = cv::Scalar::all(0);

    for( int i=0; i < m_numBlobs; ++i )
    {
        cv::Point& pos = m_positions[i];
        cv::Point& target = m_targets[i];

        // 1 when left, -1 when right
        double xBias = (target.x - pos.x);
        xBias = xBias < 0 ? -1.0 : 1.0;

        // 1 when top, -1 when bottom
        double yBias = (target.y - pos.y);
        yBias = yBias < 0 ? -1.0 : 1.0;

        pos.x += qrand() * m_factor * xBias;
        pos.y += qrand() * m_factor * yBias;

        pos.x = qMin(pos.x, img.cols);
        pos.y = qMin(pos.y, img.rows);

        if( abs(pos.x - target.x) < 5 )
        {
            target.x = (qrand()/(double)RAND_MAX) * img.cols;
        }

        if( abs(pos.y - target.y) < 5 )
        {
            target.y = (qrand()/(double)RAND_MAX) * img.rows;
        }
        cv::circle(img, pos, 10, cv::Scalar(255,255,255), CV_FILLED, CV_AA );
    }
    m_outputPin->put(out);
    return true;
}

int BlobProducer::getMaxStep() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_maxStep;
}

void BlobProducer::setMaxStep(int step)
{
    QMutexLocker lock(m_propertyMutex);
    if( step >= 0 )
    {
        m_maxStep = step;
        m_factor = (m_maxStep / (double)RAND_MAX);
    }
    emit maxStepChanged(m_maxStep);
}

int BlobProducer::getNumBlobs() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_numBlobs;
}

void BlobProducer::setNumBlobs(int num)
{
    QMutexLocker lock(m_propertyMutex);
    if( num >= 0 )
    {
        if( num > m_numBlobs )
        {
            m_positions.resize(num);
            m_targets.resize(num);
            for( int i=m_numBlobs; i < num; ++i )
            {
                m_positions[i] = cv::Point(0,0);
                m_targets[i]   = cv::Point(m_width/2, m_height/2);
            }
        }
        m_numBlobs = num;
    }
    emit numBlobsChanged(m_numBlobs);
}
