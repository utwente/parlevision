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

#ifndef IMAGECOLORCONVERT_H
#define IMAGECOLORCONVERT_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>
#include <QMutex>

namespace plv
{
    class OpenCVImage;
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class ImageColorConvert : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( ImageColorConvert )
        Q_CLASSINFO("author", "Richard")
        Q_CLASSINFO("name", "Color and scale conversion")
        Q_CLASSINFO("description", "Color and scale converion using the cv::cvtColor method. See "
                    "<a href='http://opencv.willowgarage.com/documentation/cpp/imgproc_miscellaneous_image_transformations.html?highlight=cvtcolor#cvtColor'>"
                    "OpenCV reference"
                    "</a> for a detailed explanation of the parameters.");


        Q_PROPERTY( plv::Enum conversionType READ getConversionType WRITE setConversionType NOTIFY conversionTypeChanged )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR

    public:
        ImageColorConvert();
        virtual ~ImageColorConvert();

        /** propery methods */
        plv::Enum getConversionType() { return m_conversionType; }

    signals:
        void conversionTypeChanged(plv::Enum newValue);

    public slots:
        void setConversionType(plv::Enum e);

    protected:
        int getInChannels( int code );
        int getOutChannels( int code );

        int m_inChannels;
        int m_outChannels;

        plv::CvMatDataInputPin* m_inputPin;
        plv::CvMatDataOutputPin* m_outputPin;
        plv::Enum m_conversionType;
    };
}
#endif // IMAGECOLORCONVERT_H
