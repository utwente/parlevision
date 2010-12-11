#ifndef PLVUTIL_H
#define PLVUTIL_H

#include <opencv/cv.h>
#include "Enum.h"

namespace plv
{
    class PLVCORE_EXPORT Util
    {
    public:
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
        static void addDefaultBorderInterpolationTypes( Enum& e );
    };
}

#endif
