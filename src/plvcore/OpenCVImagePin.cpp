#include "OpenCVImagePin.h"

using namespace plv;

const std::type_info& OpenCVImageInputPin::getTypeInfo() const
{
    return typeid(OpenCVImage);
}

void OpenCVImageInputPin::addSupportedDepth(int depth)
{
    m_depths.insert(depth);
}

void OpenCVImageInputPin::addSupportedChannels(int channels)
{
    m_channels.insert(channels);
}

void OpenCVImageInputPin::removeSupportedDepth(int depth)
{
    m_depths.remove(depth);
}

void OpenCVImageInputPin::removeSupportedChannels(int channels)
{
    m_channels.remove(channels);
}

bool OpenCVImageInputPin::supportsChannels( int channels ) const
{
    return m_channels.contains(channels);
}

bool OpenCVImageInputPin::supportsDepth( int depth ) const
{
    return m_depths.contains(depth);
}

void OpenCVImageInputPin::clearDephts()
{
    m_depths.clear();
}

void OpenCVImageInputPin::clearChannels()
{
    m_channels.clear();
}

void OpenCVImageInputPin::addAllDepths()
{
    m_depths.insert( IPL_DEPTH_1U );
    m_depths.insert( IPL_DEPTH_8U );
    m_depths.insert( IPL_DEPTH_8S );
    m_depths.insert( IPL_DEPTH_16U );
    m_depths.insert( IPL_DEPTH_16S );
    m_depths.insert( IPL_DEPTH_32S );
    m_depths.insert( IPL_DEPTH_32F );
    m_depths.insert( IPL_DEPTH_64F );
}

void OpenCVImageInputPin::addAllChannels()
{
    m_channels.insert( 1 );
    m_channels.insert( 2 );
    m_channels.insert( 3 );
    m_channels.insert( 4 );
}


void OpenCVImageInputPin::checkImageFormat( const RefPtr<OpenCVImage>& img )
    throw (PlvRuntimeException)
{
    if( !supportsDepth(img->getDepth()) )
    {
        QString msg = QString("Depth \"%1\" unsupported by input pin \"%2\"")
                      .arg( OpenCVImage::depthToString( img->getDepth() ) )
                      .arg( this->getName() );
        throw PlvRuntimeException( msg, __FILE__, __LINE__ );
    }

    if( !supportsChannels(img->getNumChannels()) )
    {
        QString msg = "Number of channels " %
                      QVariant(img->getNumChannels()).toString() %
                      " unsupported by input pin \"" % this->getName() %
                      "\". Supports channels (";
        QString channelsStr;
        channelsStr.reserve( (2 * m_channels.size()) + 2 );

        QList<int> channelList = m_channels.toList();
        int lastIdx = channelList.size() - 1;
        for( int i = 0; i < channelList.size(); ++i )
        {
            channelsStr += QVariant(channelList.at(i)).toString();
            if( i != lastIdx )
                channelsStr += ",";
        }
        msg += channelsStr + ").";
        throw PlvRuntimeException( msg, __FILE__, __LINE__ );
    }
}

/** Done once at connection creation only, may be slow */
bool OpenCVImageInputPin::acceptsConnectionWith( const IOutputPin* pin,
                                    QString& errStr ) const
{
    if( pin->getTypeInfo() != this->getTypeInfo() )
    {
        errStr = "Incompatible types between pins";
        return false;
    }
    const OpenCVImageOutputPin* ocvOutPin =
            dynamic_cast<const OpenCVImageOutputPin*>(pin);

    if( ocvOutPin == 0 )
    {
        errStr = "Expected OpenCVImage output pin";
        return false;
    }
    return true;
}

/******************** OpenCVImageOutputPin ********************************/

const std::type_info& OpenCVImageOutputPin::getTypeInfo() const
{
    return typeid( OpenCVImage );
}

void OpenCVImageOutputPin::addSupportedDepth(int depth)
{
    m_depths.insert( depth );
}

void OpenCVImageOutputPin::addSupportedChannels(int channels)
{
    m_channels.insert( channels );
}

void OpenCVImageOutputPin::removeSupportedDepth(int depth)
{
    m_depths.remove(depth);
}

void OpenCVImageOutputPin::removeSupportedChannels(int channels)
{
    m_channels.remove(channels);
}

bool OpenCVImageOutputPin::supportsChannels( int channels ) const
{
    return m_channels.contains(channels);
}

bool OpenCVImageOutputPin::supportsDepth( int depth ) const
{
    return m_depths.contains(depth);
}

void OpenCVImageOutputPin::clearDephts()
{
    m_depths.clear();
}

void OpenCVImageOutputPin::clearChannels()
{
    m_channels.clear();
}

void OpenCVImageOutputPin::addAllDepths()
{
    m_depths.insert( IPL_DEPTH_1U );
    m_depths.insert( IPL_DEPTH_8U );
    m_depths.insert( IPL_DEPTH_8S );
    m_depths.insert( IPL_DEPTH_16U );
    m_depths.insert( IPL_DEPTH_16S );
    m_depths.insert( IPL_DEPTH_32S );
    m_depths.insert( IPL_DEPTH_32F );
    m_depths.insert( IPL_DEPTH_64F );
}

void OpenCVImageOutputPin::addAllChannels()
{
    m_channels.insert( 1 );
    m_channels.insert( 2 );
    m_channels.insert( 3 );
    m_channels.insert( 4 );
}

void OpenCVImageOutputPin::checkImageFormat( const RefPtr<OpenCVImage>& img )
    throw (PlvRuntimeException)
{
    if( !supportsDepth(img->getDepth()) )
    {
        QString msg = "Depth unsupported by one or multiple connected input pins";
        throw PlvRuntimeException( msg, __FILE__, __LINE__ );
    }

    if( !supportsChannels(img->getNumChannels()) )
    {
        QString msg = "Number of channels " %
                      QVariant(img->getNumChannels()).toString() %
                      " unsupported by input pin \"" % this->getName() %
                      "\". Supports channels (";
        QString channelsStr;
        channelsStr.reserve( (2 * m_channels.size()) + 2 );

        QList<int> channelList = m_channels.toList();
        int lastIdx = channelList.size() - 1;
        for( int i = 0; i < channelList.size(); ++i )
        {
            channelsStr += QVariant(channelList.at(i)).toString();
            if( i != lastIdx )
                channelsStr += ",";
        }
        msg += channelsStr + ").";
        throw PlvRuntimeException( msg, __FILE__, __LINE__ );
    }
}

/** Done once at connection creation only, may be slow */
bool OpenCVImageOutputPin::acceptsConnectionWith( const IInputPin* pin,
                                    QString& errStr ) const
{
    if( pin->getTypeInfo() != this->getTypeInfo() )
    {
        errStr = "Incompatible types between pins";
        return false;
    }
    const OpenCVImageInputPin* ocvInPin =
            dynamic_cast<const OpenCVImageInputPin*>(pin);

    if( ocvInPin == 0 )
    {
        errStr = "Expected OpenCVImage input pin";
        return false;
    }
    return true;
}

OpenCVImageOutputPin* plv::createOpenCVImageOutputPin( const QString& name, PipelineElement* owner )
throw (IllegalArgumentException)
{
    // if add fails pin is automatically deleted and exception is thrown
    OpenCVImageOutputPin* pin = new OpenCVImageOutputPin( name, owner );
    owner->addOutputPin( pin );
    return pin;
}

OpenCVImageInputPin* plv::createOpenCVImageInputPin( const QString& name, PipelineElement* owner,
                                                     IInputPin::InputPinType type )
throw (IllegalArgumentException)
{
    // if add fails pin is automatically deleted and exception is thrown
    OpenCVImageInputPin* pin = new OpenCVImageInputPin( name, owner, type );
    owner->addInputPin( pin );
    return pin;
}


