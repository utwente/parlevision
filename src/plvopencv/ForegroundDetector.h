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

#ifndef ForegroundDetector_H
#define ForegroundDetector_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>
#include <plvcore/Pin.h>
#include <plvcore/CvMatData.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

class CvBGStatModel;

namespace plvopencv
{
    class ForegroundDetector : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( ForegroundDetector )
        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "ForegroundDetector")
        Q_CLASSINFO("description", "Determine foreground");

        Q_PROPERTY( plv::Enum method READ getMethod WRITE setMethod NOTIFY methodChanged )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR

    public:
        ForegroundDetector();
        virtual ~ForegroundDetector();

        plv::Enum getMethod() const;

        virtual bool start();
        virtual bool stop();

    public slots:
        void setMethod(plv::Enum method);

    signals:
        void methodChanged(plv::Enum);

    private:
        plv::CvMatDataInputPin* m_inInput;
        plv::CvMatDataOutputPin* m_outForeground;
        plv::CvMatDataOutputPin* m_outBackground;

        plv::Enum m_method;
        CvBGStatModel* m_bgModel;
    };
}
#endif // ForegroundDetector_H
