#ifndef TYPES_H
#define TYPES_H

#include <opencv/cv.h>
#include <QString>
#include "RefCounted.h"

namespace plv {

//QT_BEGIN_NAMESPACE
//class QImage;
//class QString;
//QT_END_NAMESPACE

//typedef enum _PvlPinDataType {
//    PVL_IMAGE,
//    PVL_OPENCV_IMAGE,
//    PVL_INT,
//    PVL_DOUBLE,
//    PVL_STRING
//} PvlPinDataType;
//
//#define PVL_QT_IMAGE_T PvlQtImage
//#define PVL_OPENCV_IMAGE_T PvlOpenCVImage;
//#define PVL_INT_TYPE int;
//#define PVL_DOUBLE_TYPE double;
//#define PVL_STRING_TYPE QString;

//void test( PvlPinDataType t )
//{
//    switch( t )
//    {
//    case PVL_IMAGE:
//        break;
//    case PVL_OPENCV_IMAGE:
//        break;
//    case PVL_INT:
//        break;
//    case PVL_DOUBLE:
//        break;
//    case PVL_STRING:
//        break;
//    default:
//        break;
//    }
//}

/** Base class for resources */
class Resource : public RefCounted {
public:
    /** This method compares the type and parameters of this object, with
      * the type and parameters of the other object. It returns true if the types
      * are compatible from a resource persepctive. This is not the same as
      * the typeid C++ operator, and compares additional information such as
      * private member fields to determine compatibility between two objects.
      */
    virtual bool isCompatible( const Resource* other ) = 0;
};

}

#endif // TYPES_H
