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

#ifndef IMAGESMOOTH_H
#define IMAGESMOOTH_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /*
    Smoothes image using a Gaussian filter
    Parameters:

        * src – The source image
        * dst – The destination image; will have the same size and the same type as src
        * ksize – The Gaussian kernel size; ksize.width and ksize.height can differ, but they both must be positive and odd. Or, they can be zero’s, then they are computed from sigma*
        * sigmaX, sigmaY – The Gaussian kernel standard deviations in X and Y direction. If sigmaY is zero, it is set to be equal to sigmaX . If they are both zeros, they are computed from ksize.width and ksize.height , respectively, see getGaussianKernel() . To fully control the result regardless of possible future modification of all this semantics, it is recommended to specify all of ksize , sigmaX and sigmaY
        * borderType – The pixel extrapolation method; see borderInterpolate()
    */
    class GaussianSmooth : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( GaussianSmooth )
        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "Gaussian Smooth")
        Q_CLASSINFO("description", "Smooths and image using Gaussian smoothing"
                    "See OpenCV reference for meaning of parameters."
                    "<a href='http://opencv.willowgarage.com/documentation/cpp/imgproc_image_filtering.html#GaussianBlur'>"
                    "http://opencv.willowgarage.com/documentation/cpp/imgproc_image_filtering.html#GaussianBlur"
                    "</a>")

        Q_PROPERTY( int kernelSizeWidth READ getKernelSizeWidth WRITE setKernelSizeWidth NOTIFY kernelSizeWidthChanged )
        Q_PROPERTY( int kernelSizeHeight READ getKernelSizeHeight WRITE setKernelSizeHeight NOTIFY kernelSizeHeightChanged )
        Q_PROPERTY( double sigmaOne READ getSigmaOne WRITE setSigmaOne NOTIFY sigmaOneChanged )
        Q_PROPERTY( double sigmaTwo READ getSigmaTwo WRITE setSigmaTwo NOTIFY sigmaTwoChanged )
        Q_PROPERTY( plv::Enum borderType READ getBorderType WRITE setBorderType NOTIFY borderTypeChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        GaussianSmooth();
        virtual ~GaussianSmooth();

        /** propery methods */
        int getKernelSizeWidth();
        int getKernelSizeHeight();
        double getSigmaOne();
        double getSigmaTwo();
        plv::Enum getBorderType();

    signals:
        void kernelSizeWidthChanged(int ksx);
        void kernelSizeHeightChanged(int ksy);
        void sigmaOneChanged(double s1);
        void sigmaTwoChanged(double s2);
        void borderTypeChanged(plv::Enum bt);

    public slots:
        void setKernelSizeWidth(int ksx);
        void setKernelSizeHeight(int ksy);
        void setSigmaOne(double s1);
        void setSigmaTwo(double s2);
        void setBorderType(plv::Enum bt);

    private:
        plv::CvMatDataInputPin* m_inputPin;
        plv::CvMatDataOutputPin* m_outputPin;

        int m_kernelSizeWidth;
        int m_kernelSizeHeight;
        double m_sigmaOne;
        double m_sigmaTwo;
        plv::Enum m_borderType;

    };
}
#endif // IMAGEFLIP_H
