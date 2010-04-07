#include "QtImage.h"
#include "RefPtr.h"

using namespace plv;

QtImage::QtImage( int id, QImage* img ) :
        m_id( id ),
        m_img( img )
{
}

QtImage::~QtImage()
{
    delete m_img;
}

/** Compare two opencv images for type equality */
bool QtImage::isCompatible( const Resource* other )
{
    //RefPtr<const PvlQtImage*> img = dynamic_cast<const PvlQtImage*>( other );
    const QtImage* img = dynamic_cast<const QtImage*>( other );

    if( img != 0 )
    {
        return ( m_img->width()  == img->m_img->width() &&
                 m_img->height() == img->m_img->height() &&
                 m_img->format() == img->m_img->format() );
    }
    return false;
}
