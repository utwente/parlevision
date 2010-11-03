/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvopencv module of ParleVision.
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

#ifndef IMAGETHRESHOLD_H
#define IMAGETHRESHOLD_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Types.h>
#include <plvcore/Enum.h>

namespace plv
{
    class OpenCVImage;
    class OpenCVImageInputPin;
    class OpenCVImageOutputPin;
}

namespace plvopencv
{
    /**
     * With this proccessor the received image is manipulated by thresholding
     * all the values in an image array above or below a threshold value. The
     * process either creates a binary image, truncates values to the threshold
     * or truncates to zero. It is required that the input image is in grayscale.
     */
    class ImageThreshold : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "Threshold")
        Q_CLASSINFO("description", "A processor that removes all image data above or below a given threshold."
                    "This processor uses cvThreshold, see OpenCV reference for meaning of parameters."
                    "<a href='http://opencv.willowgarage.com/documentation/miscellaneous_image_transformations.html?highlight=cvthreshold#cvThreshold'>"
                    "http://opencv.willowgarage.com/documentation/miscellaneous_image_transformations.html?highlight=cvthreshold#cvThreshold"
                    "</a>");

        Q_PROPERTY( plv::Enum method READ getMethod WRITE setMethod NOTIFY methodChanged )
        Q_PROPERTY( double threshold READ getThreshold WRITE setThreshold NOTIFY thresholdChanged )
        Q_PROPERTY( double maxValue READ getMaxValue WRITE setMaxValue NOTIFY maxValueChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        ImageThreshold();
        virtual ~ImageThreshold();

        /** propery methods */
        plv::Enum getMethod() const { return m_method; }
        double getThreshold() { return m_threshold; }
        double getMaxValue() { return m_maxValue; }

    signals:
        void methodChanged( plv::Enum m );
        void thresholdChanged(double newValue);
        void maxValueChanged(double newValue);

    public slots:
        void setMethod( plv::Enum m ) { m_method = m; emit(methodChanged(m_method));}
        void setThreshold(double d) { m_threshold = d; emit(thresholdChanged(m_threshold)); }
        void setMaxValue(double d) { m_maxValue = d; emit(maxValueChanged(m_maxValue)); }

    private:
        plv::OpenCVImageInputPin*  m_inputPin;
        plv::OpenCVImageOutputPin* m_outputPin;

        plv::Enum m_method;
        double m_threshold;
        double m_maxValue;

    };//class ImageThreshold

}//namespace plvopencv

#endif // IMAGETHRESHOLD_H
