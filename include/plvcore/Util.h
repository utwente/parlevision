#ifndef PLVUTIL_H
#define PLVUTIL_H

#include <opencv/cv.h>
#include "Enum.h"

namespace plv
{

    inline static bool isEven(int n)
    {
        return (n&1)==0;
    }

    inline static bool isOdd(int n)
    {
        return !isEven(n);
    }

    /** Adds the OpenCV border types which are use in border interpolation to a plv::enum
        See http://opencv.willowgarage.com/documentation/cpp/imgproc_image_filtering.html#borderInterpolate */
    static void addDefaultBorderInterpolationTypes( Enum& e )
    {
        e.add( "default", cv::BORDER_DEFAULT );
        e.add( "constant", cv::BORDER_CONSTANT );
        e.add( "reflect", cv::BORDER_REFLECT );
        e.add( "reflect 101", cv::BORDER_REFLECT101 );
        e.add( "reflect 101 (2)", cv::BORDER_REFLECT_101 );
        e.add( "replicate", cv::BORDER_REPLICATE );
        e.add( "wrap", cv::BORDER_WRAP );

        //e.add( "isolated",cv::BORDER_ISOLATED );
        //e.add( "transparent", cv::BORDER_TRANSPARENT );

    }
}

#endif
