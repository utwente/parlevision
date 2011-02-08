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

#include "CvMatData.h"

using namespace plv;

CvMatData::CvMatData() : d( new MatData() )
{
}

CvMatData::CvMatData( const CvMatData& other ) :
        d( other.d )
{
}

CvMatData::CvMatData( const IplImage* img )
{
    // copy image data
    // workaround for bug in OpenCV 2.0 / 2.1 which causes
    // this cv::Mat mat( img, true ); to not work.
    cv::Mat tmp(img, false);
    cv::Mat mat = tmp.clone();
    d = new MatData( mat );
}

CvMatData::CvMatData( const cv::Mat& mat, bool copyData )
{
    //d = copyData ? new MatData(mat.clone()) : new MatData(mat);
    if( copyData )
    {
        cv::Mat copy = mat.clone();
        d = new MatData(copy);
    }
    else
    {
        d = new MatData(mat);
    }
}

CvMatData CvMatData::create( int width, int height, int type )
{
    assert( width > 0 );
    assert( height > 0 );

    cv::Mat mat;
    mat.create( cv::Size(width, height), type );
    CvMatData data(mat);
    return data;
}

CvMatData::~CvMatData()
{
}

const char* CvMatData::depthToString( int depth )
{
    switch( CV_MAT_DEPTH( depth ) )
    {
    case CV_8U:
        return "CV_8U";
    case CV_8S:
        return "CV_8S";
    case CV_16U:
        return "CV_16U";
    case CV_16S:
        return "CV_16S";
    case CV_32S:
        return "CV_32S";
    case CV_32F:
        return "CV_32F";
    case CV_64F:
        return "CV_64F";
    default:
        return "Invalid depth";
    }
}

QDataStream &operator<<(QDataStream &out, const CvMatData& d)
{
    //enum { MAGIC_VAL=0x42FF0000, AUTO_STEP=0, CONTINUOUS_FLAG=CV_MAT_CONT_FLAG };
    // includes several bit-fields:
    //  * the magic signature
    //  * continuity flag
    //  * depth
    //  * number of channels
    // int flags;
    // the number of rows and columns
    // int rows, cols;
    // a distance between successive rows in bytes; includes the gap if any
    //size_t step;
    // pointer to the data
    //uchar* data;

    // pointer to the reference counter;
    // when matrix points to user-allocated data, the pointer is NULL
    //int* refcount;

    // helper fields used in locateROI and adjustROI
    //uchar* datastart;
    //uchar* dataend;

    const cv::Mat& mat = d;

    out << (qint32)mat.flags;
    out << (qint32)mat.rows;
    out << (qint32)mat.cols;

    int length = mat.dataend - mat.datastart;
    assert( length > 0 );
    out << (quint32) length;
    out.writeBytes( (const char*)mat.data, length );

    return out;
}

QDataStream &operator>>(QDataStream &in, CvMatData& d)
{
    qint32 type;
    qint32 rows;
    qint32 cols;
    quint32 len;

    in >> type;
    in >> rows;
    in >> cols;
    in >> len;

    char* data;
    uint length = (uint)len;
    in.readBytes( data, length );

    // temporary matrix, we need to explicitly copy the data because
    // using the constructor with user allocated data disables refcounting
    cv::Mat mat( (int)rows, (int)cols, (int)type );
    memcpy( mat.datastart, data, mat.dataend - mat.datastart );
    d = mat;
    delete[] data;
    return in;
}

/*
CV_BGR2BGRA     Convert BGR color to BGRA color
CV_BGRA2BGR     Convert BGRA color to BGR color
CV_BGR2GRAY     Convert BGR color to GRAY color
CV_GRAY2BGR     Convert GRAY color to BGR color
CV_GRAY2BGRA    Convert GRAY color to BGRA color
CV_BGRA2GRAY    Convert BGRA color to GRAY color
CV_BGR2BGR565   Convert BGR color to BGR565 color
CV_BGR5652BGR   Convert BGR565 color to BGR color
CV_BGRA2BGR565  Convert BGRA color to BGR565 color
CV_BGR5652BGRA  Convert BGR565 color to BGRA color
CV_GRAY2BGR565  Convert GRAY color to BGR565 color
CV_BGR5652GRAY  Convert BGR565 color to GRAY color
CV_BGR2BGR555   Convert BGR color to BGR555 color
CV_BGR5552BGR   Convert BGR555 color to BGR color
CV_BGRA2BGR555  Convert BGRA color to BGR555 color
CV_BGR5552BGRA  Convert BGR555 color to BGRA color
CV_GRAY2BGR555  Convert GRAY color to BGR555 color
CV_BGR5552GRAY  Convert BGR555 color to GRAY color
CV_BGR2XYZ      Convert BGR color to XYZ color
CV_XYZ2BGR      Convert XYZ color to BGR color
CV_BGR2YCrCb    Convert BGR color to YCrCb color
CV_YCrCb2BGR    Convert YCrCb color to BGR color
CV_BGR2HSV      Convert BGR color to HSV color
CV_BGR2Lab      Convert BGR color to Lab color
CV_BayerBG2BGR  Convert BayerBG color to BGR color
CV_BayerGB2BGR  Convert BayerGB color to BGR color
CV_BayerRG2BGR  Convert BayerRG color to BGR color
CV_BayerGR2BGR  Convert BayerGR color to BGR color
CV_BGR2Luv      Convert BGR color to Luv color
CV_BGR2HLS      Convert BGR color to HLS color
CV_HSV2BGR      Convert HSV color to BGR color
CV_Lab2BGR      Convert Lab color to BGR color
CV_Luv2BGR      Convert Luv color to BGR color
CV_HLS2BGR      Convert HLS color to BGR color
*/
