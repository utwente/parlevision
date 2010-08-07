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

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    class ImageColorConvert : public PipelineProcessor
    {
        Q_OBJECT
        //Q_PROPERTY( int conversionType READ getConversionType WRITE setConversionType NOTIFY conversionType )
        Q_PROPERTY( plv::Enum conversionType READ getConversionType WRITE setConversionType NOTIFY conversionTypeChanged )

    public:
        ImageColorConvert();
        ~ImageColorConvert();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        plv::Enum getConversionType() { return m_conversionType; }

    signals:
        void conversionTypeChanged(plv::Enum newValue);

    public slots:
        void setConversionType(plv::Enum e) { m_conversionType = e; emit(conversionTypeChanged(e)); }

    private:
        int nearestOdd(int i);

        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;

        plv::Enum m_conversionType;
    };
}
#endif // IMAGECOLORCONVERT_H
