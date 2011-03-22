#include "Util.h"

using namespace plv;

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

