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

#ifndef IMAGECORNERHARRIS_H
#define IMAGECORNERHARRIS_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class ImageCornerHarris : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( ImageCornerHarris )
        Q_CLASSINFO("author", "Richard")
        Q_CLASSINFO("name", "Harris corner detection")
        Q_CLASSINFO("description", "Harris corner detection using the cv::cornerHarris method. See "
                    "<a href='http://opencv.willowgarage.com/documentation/cpp/imgproc_feature_detection.html?highlight=harris#cornerHarris'>"
                    "OpenCV reference"
                    "</a> for a detailed explanation of the parameters.");

        Q_PROPERTY( int kernelSize READ getKernelSize WRITE setKernelSize NOTIFY kernelSizeChanged )
        Q_PROPERTY( int blockSize READ getBlockSize WRITE setBlockSize NOTIFY blockSizeChanged )
        Q_PROPERTY( double k READ getK WRITE setK NOTIFY kChanged )
        Q_PROPERTY( plv::Enum borderType READ getBorderType WRITE setBorderType NOTIFY borderTypeChanged )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR

    public:
        ImageCornerHarris();
        virtual ~ImageCornerHarris();

        /** propery methods */
        int getKernelSize() const;
        int getBlockSize() const;
        double getK() const;
        plv::Enum getBorderType() const;

    signals:
        void kernelSizeChanged(int newValue);
        void blockSizeChanged(int newValue);
        void kChanged(double newValue);
        void borderTypeChanged(plv::Enum e);

    public slots:
        void setKernelSize(int i);
        void setBlockSize(int i);
        void setK (double newValue);
        void setBorderType( plv::Enum e );

    private:
        plv::CvMatDataInputPin* m_inputPin;
        plv::CvMatDataOutputPin* m_outputPin;

        int m_kernelSize;
        int m_blockSize;
        double m_k;
        plv::Enum m_borderType;
    };
}
#endif // IMAGECORNERHARRIS_H
