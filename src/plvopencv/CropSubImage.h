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

#ifndef CROPSUBIMAGE_H
#define CROPSUBIMAGE_H

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
    class CropSubImage : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( CropSubImage )
        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "Copy and crop ROIs")
        Q_CLASSINFO("description", "Copies and crops one or several ROIs. s"
                    "Can be used together with Viola Jones Face detector for instance.")

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR
    public:
        CropSubImage();
        virtual ~CropSubImage();
    private:
        plv::CvMatDataInputPin* m_imageInput;
        plv::InputPin< plv::RectangleData >* m_regions;
        plv::OutputPin< QList<plv::CvMatData> >* m_imagesOutput;
    };
}

#endif // CROPSUBIMAGE_H
