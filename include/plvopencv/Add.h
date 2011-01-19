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

#ifndef ADD_H
#define ADD_H

#include <plvcore/PipelineProcessor.h>

namespace plv
{
    class OpenCVImage;
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /**
      * Add two images.
      */
    class Add : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( Add )
        Q_CLASSINFO("author", "Richard")
        Q_CLASSINFO("name", "A add B")
        Q_CLASSINFO("description", "Computes the weighted sum of two arrays.\n"
                    "Parameters: \n"
                        "alpha – Weight for the first array elements \n"
                        "beta – Weight for the second array elements \n"
                        "gamma – Scalar, added to each sum \n"
                        "See <a href='http://opencv.jp/opencv-2.1_org/cpp/operations_on_arrays.html'>here</a> "
                        "for a detailed description" )

        Q_PROPERTY( double alpha READ getAlpha WRITE setAlpha NOTIFY alphaChanged )
        Q_PROPERTY( double beta READ getBeta WRITE setBeta NOTIFY betaChanged )
        Q_PROPERTY( double gamma READ getGamma WRITE setGamma NOTIFY gammaChanged )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR

    public:
        Add();
        virtual ~Add();

        /** propery methods */
        double getAlpha() const;
        double getBeta() const;
        double getGamma() const;

    signals:
        void alphaChanged(double d);
        void betaChanged(double d);
        void gammaChanged(double d);

    public slots:
        void setAlpha(double a, bool doEmit=false);
        void setBeta(double b, bool doEmit=false);
        void setGamma(double g, bool doEmit=false);

    private:
        plv::CvMatDataInputPin* m_inputPin1;
        plv::CvMatDataInputPin* m_inputPin2;
        plv::CvMatDataOutputPin* m_outputPin;

        double m_alpha;
        double m_beta;
        double m_gamma;
    };
}
#endif // ADD_H
