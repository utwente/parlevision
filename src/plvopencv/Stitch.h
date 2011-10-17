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

#ifndef STITCH_H
#define STITCH_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/CvMatData.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

#define STITCH_DESTINATION_WIDTH_DEFAULT (640*2)
#define STITCH_DESTINATION_HEIGHT_DEFAULT (480*2)

#define STITCH_DESTINATION_WIDTH_MAX (1920*2)
#define STITCH_DESTINATION_HEIGHT_MAX (1080*2)

namespace plvopencv
{
    /** Takes one multi channel image and splits it in multiple single channel images. */
    class Stitch : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "Stitch")
        Q_CLASSINFO("description", "")

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_PROCESSOR

        Q_PROPERTY( int destinationWidth READ getDestinationWidth WRITE setDestinationWidth NOTIFY destinationWidthChanged )
        Q_PROPERTY( int destinationHeight READ getDestinationHeight WRITE setDestinationHeight NOTIFY destinationHeightChanged )

        Q_PROPERTY( int in0X READ getIn0X WRITE setIn0X NOTIFY in0XChanged )
        Q_PROPERTY( int in0Y READ getIn0Y WRITE setIn0Y NOTIFY in0YChanged )

        Q_PROPERTY( int in1X READ getIn1X WRITE setIn1X NOTIFY in1XChanged )
        Q_PROPERTY( int in1Y READ getIn1Y WRITE setIn1Y NOTIFY in1YChanged )

        Q_PROPERTY( int in2X READ getIn2X WRITE setIn2X NOTIFY in2XChanged )
        Q_PROPERTY( int in2Y READ getIn2Y WRITE setIn2Y NOTIFY in2YChanged )

        Q_PROPERTY( int in3X READ getIn3X WRITE setIn3X NOTIFY in3XChanged )
        Q_PROPERTY( int in3Y READ getIn3Y WRITE setIn3Y NOTIFY in3YChanged )

        Q_PROPERTY( bool blend READ getBlend WRITE setBlend NOTIFY blendChanged )


    public:
        Stitch();
        virtual ~Stitch();

        int getDestinationWidth();
        int getDestinationHeight();

        int getIn0X();
        int getIn0Y();
        int getIn1X();
        int getIn1Y();
        int getIn2X();
        int getIn2Y();
        int getIn3X();
        int getIn3Y();
        bool getBlend();

    public slots:
        void setIn0X( int value );
        void setIn0Y( int value );
        void setIn1X( int value );
        void setIn1Y( int value );
        void setIn2X( int value );
        void setIn2Y( int value );
        void setIn3X( int value );
        void setIn3Y( int value );

        void setDestinationHeight( int height );
        void setDestinationWidth( int width );

        void setBlend( bool blend );

    signals:
        void in0XChanged( int value );
        void in1XChanged( int value );
        void in2XChanged( int value );
        void in3XChanged( int value );

        void in0YChanged( int value );
        void in1YChanged( int value );
        void in2YChanged( int value );
        void in3YChanged( int value );

        void destinationHeightChanged( int height );
        void destinationWidthChanged( int width );

        void blendChanged( bool blend );

    private:
        void copyImgInto( const cv::Mat& in, cv::Mat& out, int posX, int posY );

        plv::CvMatDataInputPin*  m_inputPin0;
        plv::CvMatDataInputPin*  m_inputPin1;
        plv::CvMatDataInputPin*  m_inputPin2;
        plv::CvMatDataInputPin*  m_inputPin3;

        plv::CvMatDataOutputPin* m_outputPin;

        int destinationWidth;
        int destinationHeight;

        int in0x;
        int in0y;
        int in1x;
        int in1y;
        int in2x;
        int in2y;
        int in3x;
        int in3y;

        bool m_blend;
    };
}

#endif // STITCH_H
