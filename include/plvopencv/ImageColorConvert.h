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
    class OpenCVImageInputPin;
    class OpenCVImageOutputPin;
}

namespace plvopencv
{
    class ImageColorConvert : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_PROPERTY( plv::Enum conversionType READ getConversionType WRITE setConversionType NOTIFY conversionTypeChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

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

        plv::OpenCVImageInputPin* m_inputPin;
        plv::OpenCVImageOutputPin* m_outputPin;
        plv::Enum m_conversionType;
        QMutex m_colorConvertMutex;
    };
}
#endif // IMAGECOLORCONVERT_H
