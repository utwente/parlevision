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

#include "TestProducer.h"

#include <QDebug>
#include <plvcore/Pin.h>
#include <plvcore/CvMatDataPin.h>

using namespace plv;

TestProducer::TestProducer()
{
    m_intOut = createOutputPin<int>("int", this);
    m_stringOut = createOutputPin<QString>("QString", this);
    m_floatOut = createOutputPin<float>("float", this);
    m_doubleOut = createOutputPin<double>("double", this);

    m_16bitSingleChannelImageOut = createCvMatDataOutputPin("CV_16C1", this);
    m_32bitSingleChannelImageOut = createCvMatDataOutputPin("CV_32F1", this);
}

TestProducer::~TestProducer()
{
}

bool TestProducer::init()
{
    return true;
}

bool TestProducer::deinit() throw ()
{
    return true;
}

bool TestProducer::start()
{
    return true;
}

bool TestProducer::stop()
{
    return true;
}

bool TestProducer::readyToProduce() const
{
    return true;
}

bool TestProducer::produce()
{
    unsigned int serial = getProcessingSerial();

    m_intOut->put(int(serial));
    m_stringOut->put(QString("%1").arg(serial));
    m_floatOut->put(float(serial));
    m_doubleOut->put(double(serial));

    CvMatData bit16UChan1 = CvMatData::create(800,600,CV_16U,1);
    CvMatData bit32FChan1 = CvMatData::create(800,600,CV_32F,1);

    {
        cv::Mat& mat16u = bit16UChan1;
        for (int y = 0; y < mat16u.rows; ++y )
        {
            for (int x = 0; x < mat16u.cols; ++x )
            {
                mat16u.at<uint16_t>(y,x) = qrand() % 2048;
            }
        }

        for (int y = 0; y < mat16u.rows; ++y )
        {
            for (int x = 0; x < mat16u.cols; ++x )
            {
                uint16_t val = mat16u.at<uint16_t>(y,x);
                mat16u.at<uint16_t>(y,x) = val << 5;
            }
        }
    }

    {
        cv::Mat& mat32f = bit32FChan1;
        int half = mat32f.rows / 2;
        for (int y = 0; y < mat32f.rows; ++y )
        {
            float val = y > half ? 1.0f : 0.0f;
            for (int x = 0; x < mat32f.cols; ++x )
            {
                mat32f.at<float>(y,x) = val;
            }
        }
    }
    //mat *= (USHRT_MAX / 2048); 2^16 / 2^11 = 2^5 = 32




    m_16bitSingleChannelImageOut->put(bit16UChan1);
    m_32bitSingleChannelImageOut->put(bit32FChan1);
    return true;
}
