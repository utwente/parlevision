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

#ifndef BackgroundSubtractor_H
#define BackgroundSubtractor_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>
#include <plvcore/InputPin.h>
#include <plvcore/CvMatData.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class BackgroundSubtractor : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( BackgroundSubtractor )
        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "BackgroundSubtractor")
        Q_CLASSINFO("description", "Subtract background from foreground");

        Q_PROPERTY( plv::Enum method READ getMethod WRITE setMethod NOTIFY methodChanged )
        Q_PROPERTY( bool reset READ getReset WRITE setReset NOTIFY resetChanged )
        Q_PROPERTY( int replacement READ getReplacement WRITE setReplacement NOTIFY replacementChanged )
        Q_PROPERTY( int threshold READ getThreshold WRITE setThreshold NOTIFY thresholdChanged )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR

    public:
        BackgroundSubtractor();
        virtual ~BackgroundSubtractor();

        int getThreshold() const;
        int getReplacement() const;
        bool getReset() const;
        plv::Enum getMethod() const;

    public slots:
        void setReplacement(int r);
        void setThreshold(int t);
        void setReset(bool r);
        void setMethod(plv::Enum method);

    signals:
        void thresholdChanged(int t);
        void replacementChanged(int r);
        void resetChanged(bool);
        void methodChanged(plv::Enum);

    private:
        plv::CvMatDataInputPin* m_inInput;
        plv::InputPin<bool>* m_inReset;
        plv::CvMatDataInputPin* m_inBackground;
        plv::CvMatDataOutputPin* m_outForeground;
        plv::CvMatDataOutputPin* m_outBackground;
        plv::CvMatData m_background;
        plv::CvMatData m_backgroundGray;
        int m_threshold;
        int m_replacement;
        plv::Enum m_method;
        bool m_reset;

        void setBackground(plv::CvMatData& mat);
    };
}
#endif // BackgroundSubtractor_H
