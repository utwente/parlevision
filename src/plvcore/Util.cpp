#include "Util.h"

using namespace plv;

QString Util::m_buildDate = __DATE__;
QString Util::m_buildTime = __TIME__;
QString Util::m_buildType =
#ifdef DEBUG
        "debug";
#else
        "release";
#endif

#if defined _MSC_VER
    // TODO test on actual Windows env
    QString Util::m_compilerName          =  "MSC";
    QString fullVersion                   = _MSC_FULL_VER;
    int Util::m_compilerVersionMajor      = (int) subString(fullVersion, 0, 2);
    int Util::m_compilerVersionMinor      = (int) subString(fullVersion, 2, 4);
    int Util::m_compilerVersionPatchLevel = (int) subString(fullVersion, 4);
#elif defined __GNUC__
    QString Util::m_compilerName = QString("GCC");
    int Util::m_compilerVersionMajor      = __GNUC__;
    int Util::m_compilerVersionMinor      = __GNUC_MINOR__;
    int Util::m_compilerVersionPatchLevel = __GNUC_PATCHLEVEL__;
#else
    QString Util::m_compilerName = QString("Unknown");
#endif

QString Util::m_buildInformation = QString("[%1 %2][%3 %4.%5 %6]").arg( m_buildDate )
                                                            .arg( m_buildTime )
                                                            .arg( m_compilerName )
                                                            .arg( m_compilerVersionMajor )
                                                            .arg( m_compilerVersionMinor )
                                                            .arg( m_buildType );

void Util::addDefaultBorderInterpolationTypes( plv::Enum& e )
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

bool overlap( const cv::Rect& a, const cv::Rect& b)
{
    cv::Point pa[2];
    pa[0].x = a.x;
    pa[0].y = a.y;
    pa[1].x = a.x + a.width;
    pa[1].y = a.y + a.height;

    cv::Point pb[2];
    pb[0].x = b.x;
    pb[0].y = b.y;
    pb[1].x = b.x + b.width;
    pb[1].y = b.y + b.height;

    return(pa[0].x < pb[1].x && pa[1].x > pb[0].x && pa[0].y < pb[1].y && pa[1].y > pb[0].y);
}

int overlappingArea( const cv::Rect& a, const cv::Rect& b, cv::Rect& area )
{
    //A.X1 < B.X2: 	true
    //A.X2 > B.X1: 	true
    //A.Y1 < B.Y2: 	true
    //A.Y2 > B.Y1: 	true

    cv::Point2i pa[2];
    pa[0].x = a.x;
    pa[0].y = a.y;
    pa[1].x = a.x + a.width;
    pa[1].y = a.y + a.height;

    cv::Point2i pb[2];
    pb[0].x = b.x;
    pb[0].y = b.y;
    pb[1].x = b.x + b.width;
    pb[1].y = b.y + b.height;

    if(!(pa[0].x < pb[1].x && pa[1].x > pb[0].x && pa[0].y < pb[1].y && pa[1].y > pb[0].y))
    {
        // no matching area
        return 0;
    }

    // matching area, calculate the two points of
    // the matching rectangle

    // take max of minimum coordinates
    cv::Point x1y1;
    x1y1.x = pa[0].x > pb[0].x ? pa[0].x : pb[0].x;
    x1y1.y = pa[0].y > pb[0].y ? pa[0].y : pb[0].y;

    // take min of maximum coordinates
    cv::Point x2y2;
    x2y2.x = pa[1].x < pb[1].x ? pa[1].x : pb[1].x;
    x2y2.y = pa[1].y < pb[1].y ? pa[1].y : pb[1].y;

    area.x = x1y1.x;
    area.y = x1y1.y;
    area.width = x2y2.x - x1y1.x;
    area.height = x2y2.y - x1y1.y;
    return true;
}

int Math::factorial(int n)
{
    int result=n;
    for(int i=2;i<n;++i)
        result*=i;
    return result;
}

