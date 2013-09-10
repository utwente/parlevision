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

        static const QString& getBuildInformation() { return m_buildInformation; }
        static const QString& getBuildDate() { return m_buildDate; }
        static const QString& getBuildTime() { return m_buildTime; }
        static const QString& getBuildType() { return m_buildType; }
        static const QString& getCompilerName() { return m_compilerName; }
        static int getCompilerVersionMajor() { return m_compilerVersionMajor; }
        static int getCompilerVersionMinor() { return m_compilerVersionMinor; }
        static int getCompilerVersionPatchLevel() { return m_compilerVersionPatchLevel; }

    private:
        static QString m_buildInformation;
        static QString m_buildDate;
        static QString m_buildTime;
        static QString m_buildType;
        static QString m_compilerName;
        static int m_compilerVersionMajor;
        static int m_compilerVersionMinor;
        static int m_compilerVersionPatchLevel;
    };

    class PLVCORE_EXPORT Math
    {
    public:
        static int factorial(int n);
    };
}

bool PLVCORE_EXPORT overlap(const cv::Rect& a, const cv::Rect& b);
int  PLVCORE_EXPORT overlappingArea(const cv::Rect& a, const cv::Rect& b, cv::Rect& area);

#endif
