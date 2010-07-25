#include "QtImage.h"
#include "RefPtr.h"

using namespace plv;

QtImage::QtImage( QImage img ) :
        Data(),
        m_img( img )
{
}

QtImage::QtImage( const QtImage& other ) :
        Data(),
        m_img( other.m_img )
{
}

QtImage::~QtImage()
{
}

/** Compare two opencv images for type equality */
//bool QtImage::isCompatible( const ComplexData* other )
//{
//    //RefPtr<const PvlQtImage*> img = dynamic_cast<const PvlQtImage*>( other );
//    const QtImage* img = dynamic_cast<const QtImage*>( other );
//
//    if( img != 0 )
//    {
//        return ( m_img->width()  == img->m_img->width() &&
//                 m_img->height() == img->m_img->height() &&
//                 m_img->format() == img->m_img->format() );
//    }
//    return false;
//}
