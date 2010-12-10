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

#ifndef EDGEDETECTORSOBEL_H
#define EDGEDETECTORSOBEL_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /** Calculates the first, second, third or mixed image derivatives using an extended Sobel operator */
    class EdgeDetectorSobel : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( EdgeDetectorSobel )
        Q_CLASSINFO("author", "Richard, Wim, Dennis")
        Q_CLASSINFO("name", "Sobel operator")
        Q_CLASSINFO("description", "Calculates the first, second, third or mixed image derivatives using an extended Sobel operator.");

        Q_PROPERTY( bool xOrder READ getXOrder WRITE setXOrder NOTIFY xOrderChanged )
        Q_PROPERTY( bool yOrder READ getYOrder WRITE setYOrder NOTIFY yOrderChanged )
        Q_PROPERTY( int kernelSize READ getKernelSize WRITE setKernelSize NOTIFY kernelSizeChanged )
        Q_PROPERTY( double scale READ getScale WRITE setScale NOTIFY scaleChanged )
        Q_PROPERTY( double delta READ getDelta WRITE setDelta NOTIFY deltaChanged )
        Q_PROPERTY( plv::Enum borderType READ getBorderType WRITE setBorderType NOTIFY borderTypeChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        EdgeDetectorSobel();
        virtual ~EdgeDetectorSobel();

        /** propery methods */
        bool getXOrder() const;
        bool getYOrder() const;
        int getKernelSize() const;
        double getScale() const;
        double getDelta() const;
        plv::Enum getBorderType() const;

    signals:
        void xOrderChanged(bool x);
        void yOrderChanged(bool y);
        void kernelSizeChanged(int ks);
        void scaleChanged(double s);
        void deltaChanged(double d);
        void borderTypeChanged(plv::Enum e);

    public slots:
        void setKernelSize(int a);
        void setXOrder( bool x );
        void setYOrder( bool y );
        void setScale( double s );
        void setDelta( double d );
        void setBorderType( plv::Enum e );

    private:
        plv::CvMatDataInputPin* m_inputPin;
        plv::CvMatDataOutputPin* m_outputPin;

        bool m_xOrder; /** Order of the derivative x */
        bool m_yOrder; /** Order of the derivative y */
        int m_kernelSize; /** Size of the extended Sobel kernel, must be 1, 3, 5 or 7 */
        double m_scale; /** The optional scale factor for the computed derivative values (by default, no scaling is applied */
        double m_delta; /** The optional delta value, added to the results prior to storing them in destination */
        plv::Enum m_borderType; /** The pixel extrapolation method  */
    };
}
#endif // EDGEDETECTORSOBEL_H
