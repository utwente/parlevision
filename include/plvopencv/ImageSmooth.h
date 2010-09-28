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
#include <plvcore/Pin.h>

namespace plvopencv
{
    class OpenCVImage;

    class ImageSmooth : public plv::PipelineProcessor
    {
        Q_OBJECT
        
        Q_CLASSINFO("author", "Dennis & Wim")
        Q_CLASSINFO("name", "Smooth")
        Q_CLASSINFO("description", "Smoothing using cvSmooth."
                    "See OpenCV reference for meaning of parameters."
                    "<a href='http://opencv.willowgarage.com/documentation/image_filtering.html?highlight=cvsmooth#cvSmooth'>"
                    "http://opencv.willowgarage.com/documentation/image_filtering.html?highlight=cvsmooth#cvSmooth"
                    "</a>")

        Q_PROPERTY( plv::Enum method READ getMethod WRITE setMethod NOTIFY methodChanged )

        Q_PROPERTY( int one READ getOne WRITE setOne NOTIFY oneChanged )
        Q_PROPERTY( int two READ getTwo WRITE setTwo NOTIFY twoChanged )
        Q_PROPERTY( double three READ getThree WRITE setThree NOTIFY threeChanged )
        Q_PROPERTY( double four READ getFour WRITE setFour NOTIFY fourChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        ImageSmooth();
        ~ImageSmooth();

        /** propery methods */
        plv::Enum getMethod() const { return m_method; }
        int getOne() { return m_one; }
        int getTwo() { return m_two; }
        double getThree() { return m_three; }
        double getFour() { return m_four; }

    signals:
        void oneChanged(int newValue);
        void twoChanged(int newValue);
        void threeChanged(double newValue);
        void fourChanged(double newValue);
        void methodChanged( plv::Enum m );

    public slots:
        void setOne(int i);
        void setTwo(int i);
        void setThree(double i) { m_three = i; emit(threeChanged(i)); }
        void setFour(double i) { m_four = i; emit(fourChanged(i)); }
        void setMethod( plv::Enum m ) { m_method = m; emit(methodChanged(m_method));}

    private:

        plv::InputPin<OpenCVImage>* m_inputPin;
        plv::OutputPin<OpenCVImage>* m_outputPin;

        plv::Enum m_method;
        int m_one;
        int m_two;
        double m_three;
        double m_four;
    };
}
#endif // IMAGEFLIP_H
