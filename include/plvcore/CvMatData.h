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
    /** internal class to CvMatData. Not exported */
    class MatData : public QSharedData
    {
    public:
        inline MatData( const cv::Mat& mat ) : mat(mat) {}

        // explicit copy of matrix header and data
        inline MatData( const MatData& other ) { mat = other.mat.clone(); }
        inline ~MatData() {}

        /** actual opencv matrix */
        cv::Mat mat;
    };

    class PLVCORE_EXPORT CvMatData : public Data
    {

    public:
        explicit CvMatData( const cv::Mat& mat, bool copyData = false )
        {
            d = copyData ? new MatData(mat.clone()) : new MatData(mat);
        }
        CvMatData( const CvMatData& other ) : Data(other), d( other.d ) {}
        ~CvMatData();

        /** Returns if the contained matrix has allocated data */
        inline bool isValid() const { return d->mat.data != 0; }

        /** Returns a copy of the cv::Mat header. The actual image data is not copied
            Warning: do not remove const qualifier since this data might be shared
            between threads. Beware, this can be done implicitly e.g. by converting to
            IplImage using the operator IplImage! */
        inline const cv::Mat get() const { return d->mat; }

        /** returns a header which references the internal cv::Mat data.
            When the reference count is larger than 1 a copy is made of
            the matrix data. */
        inline cv::Mat get() { return d->mat; }

        // constructs matrix of the specified size and type
        // (_type is CV_8UC1, CV_64FC3, CV_32SC(12) etc.)
        static CvMatData create( int width, int height, int type );

        inline int type() const { return d->mat.type(); }
        inline int channels() const { return d->mat.channels(); }
        inline int width() const { return d->mat.cols; }
        inline int height() const { return d->mat.rows; }

    private:
        /** the internal open cv matrix */
        QSharedDataPointer<MatData> d;
    };
}

#endif
