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

#ifndef CVMATDATA_H
#define CVMATDATA_H

#include <list>
#include <QMutex>
#include <QSharedData>
#include <opencv/cv.h>

#include "Types.h"
#include "PlvExceptions.h"

#define CVMATDATA_MAX_OBJECT_POOL_SIZE (1024 * 1024 * 8)
#define CVMATDATA_USE_POOL 1

namespace plv
{
    /** helper class for dealing with properties of the cv::Mat */
    class PLVCORE_EXPORT CvMatDataProperties
    {
    protected:
        int m_width;
        int m_height;
        int m_type; // combination of depth and channels

    public:
        inline CvMatDataProperties( int width,int height, int type ) :
                m_width(width), m_height(height),
                m_type(type) {}

        inline CvMatDataProperties( const cv::Mat& mat ) :
                m_width(mat.cols), m_height(mat.rows), m_type(mat.type()) {}

        inline bool operator == (const CvMatDataProperties& rhs ) const
        {
            return m_width == rhs.m_width &&
                   m_height == rhs.m_height &&
                   m_type == rhs.m_type;
        }

        inline bool operator != (const CvMatDataProperties& rhs ) const
        {
            return m_width != rhs.m_width ||
                   m_height != rhs.m_height ||
                   m_type != rhs.m_type;
        }

        inline int width() const { return m_width; }
        inline int height() const { return m_height; }
        inline int depth() const { return CV_MAT_DEPTH( m_type ); }
        inline int channels() const { return CV_MAT_CN( m_type ); }
        inline int type() const { return m_type; }

        inline void setWidth( int width ) { m_width = width; }
        inline void setHeight( int height ) { m_height = height; }
        inline void setDepth( int depth ) { m_type = CV_MAKE_TYPE(depth,  CV_MAT_CN( m_type )); }
        inline void setNumChannels( int numChannels ) { m_type = CV_MAKE_TYPE( CV_MAT_DEPTH( m_type ), numChannels); }
        inline void setType( int type ) { m_type = type; }
    };

    /** internal class to CvMatData. Not exported */
    class MatData : public QSharedData
    {
    public:
        inline MatData() : mat(cv::Mat()) { inputArray = mat; outputArray = mat; }
        inline MatData( const cv::Mat& nmat ) : mat(nmat), inputArray(mat), outputArray(mat){}

        // explicit copy of matrix header and data
        inline MatData( const MatData& other ) : QSharedData()
        {
            mat = other.mat.clone();
            inputArray = mat;
            outputArray = mat;
        }
        inline ~MatData() {}

        /** actual opencv matrix */
        cv::Mat mat;

        /** needed for conversions */
        cv::_InputArray inputArray;
        cv::_OutputArray outputArray;
    };

    class PLVCORE_EXPORT CvMatData
    {

    public:
        inline CvMatData() : d( new MatData() ) {}
        CvMatData( const cv::Mat& mat, bool copyData = false );
        CvMatData( const IplImage* img );

        /** copy constructor */
        inline CvMatData( const CvMatData& other ) : d( other.d ) {}

        /** destructor */
        inline ~CvMatData() {}

        CvMatData& operator=(const CvMatData& other);
        CvMatData& operator=(const cv::Mat& other);
        CvMatData& operator=(const IplImage* iplimg);

        inline void copyTo(CvMatData& dst) { d->mat.copyTo(dst); }

        /** Returns if the contained matrix has allocated data */
        inline bool isValid() const { return !d->mat.empty(); }
        inline bool isEmpty() const { return d->mat.empty(); }

        /** Returns a copy of the cv::Mat header. The actual image data is not copied
            Warning: do not remove const qualifier since this data might be shared
            between threads. Beware, this can be done implicitly e.g. by converting to
            IplImage using the operator IplImage! */
        inline const cv::Mat get() const { return d->mat; }

        /** returns a header which references the internal cv::Mat data.
            When the reference count is larger than 1 a copy is made of
            the matrix data. */
        inline cv::Mat get() { return d->mat; }

        /** constructs matrix of the specified size and type
          ( depthAndChannels is CV_8UC1, CV_64FC3, CV_32SC(12) etc.)
            or CV_MAKETYPE( depth, channels ) where depth is CV_8U etc */
        static CvMatData create( int width, int height, int depthAndChannels );

        inline static CvMatData create( int width, int height, int depth, int channels )
        {
            return create( width, height, CV_MAKETYPE( depth, channels ) );
        }
        inline static CvMatData create( const CvMatDataProperties& props )
        {
            return create( props.width(), props.height(), props.type() );
        }

        inline int type() const { return d->mat.type(); }
        inline int depth() const { return d->mat.depth(); }
        inline int channels() const { return d->mat.channels(); }
        inline int width() const { return d->mat.cols; }
        inline int height() const { return d->mat.rows; }
        inline int cols() const { return d->mat.cols; }
        inline int rows() const { return d->mat.rows; }
        inline CvMatDataProperties properties() const { return CvMatDataProperties(d->mat); }

        static const char* depthToString( int depth );

        inline operator cv::Mat&() { return d->mat; }
        inline operator const cv::Mat&() const { return d->mat; }

        inline operator cv::InputArray() const { return d->inputArray; }
        inline operator const cv::_OutputArray&() { return d->outputArray; }

    private:
        /** the internal open cv matrix */
        QSharedDataPointer<MatData> d;
    };

    inline CvMatData& CvMatData::operator=(const CvMatData& other)
    {
        if( this != &other )
        {
            d = other.d;
        }
        return *this;
    }

    inline CvMatData& CvMatData::operator=(const cv::Mat& other)
    {
        if( &d->mat != &other )
        {
            d->mat = other;
            d->inputArray = d->mat;
            d->outputArray = d->mat;
        }
        return *this;
    }

    inline CvMatData& CvMatData::operator=(const IplImage* iplimg)
    {
        assert( iplimg != 0 );
        // copy image data
        // workaround for bug in OpenCV 2.0 / 2.1 which causes
        // this cv::Mat mat( img, true ); to not work.
        cv::Mat tmp(iplimg, false);
        cv::Mat mat = tmp.clone();
        d->mat = mat;
        d->inputArray = d->mat;
        d->outputArray = d->mat;
        return *this;
    }
}
PLVCORE_EXPORT QDataStream& operator<<(QDataStream &out, const plv::CvMatData& d);
PLVCORE_EXPORT QDataStream& operator>>(QDataStream &in, plv::CvMatData& d);

Q_DECLARE_METATYPE( plv::CvMatData )
Q_DECLARE_METATYPE( QList<plv::CvMatData> )

#endif
