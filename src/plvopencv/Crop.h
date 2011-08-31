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

#ifndef CROPIMAGE_H
#define CROPIMAGE_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Pin.h>
#include <plvcore/Types.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
    class CvMatData;
}

namespace plvopencv
{
    class CropImage : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( CropImage )
        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "Crop image")
        Q_CLASSINFO("description", "Crops and image")

        Q_PROPERTY( int x1 READ getX1 WRITE setX1 NOTIFY x1Changed )
        Q_PROPERTY( int x2 READ getX2 WRITE setX2 NOTIFY x2Changed )
        Q_PROPERTY( int y1 READ getY1 WRITE setY1 NOTIFY y1Changed )
        Q_PROPERTY( int y2 READ getY2 WRITE setY2 NOTIFY y2Changed )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR
    public:
        CropImage();
        virtual ~CropImage();

        int getX1();
        int getX2();
        int getY1();
        int getY2();

    public slots:
        void setX1(int x1);
        void setX2(int x2);
        void setY1(int y1);
        void setY2(int y2);

    signals:
        void x1Changed(int x1);
        void x2Changed(int x2);
        void y1Changed(int y1);
        void y2Changed(int y2);

    private:
        plv::CvMatDataInputPin* m_in;
        plv::CvMatDataOutputPin* m_out;

        int m_x1;
        int m_x2;
        int m_y1;
        int m_y2;
    };
}

#endif // CROPIMAGE_H
