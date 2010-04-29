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
