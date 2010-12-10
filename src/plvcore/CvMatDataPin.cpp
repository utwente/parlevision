#include "CvMatDataPin.h"

#include <QMetaType>

using namespace plv;

const std::type_info& CvMatDataInputPin::getTypeInfo() const
{
    return typeid(CvMatData);
}

/** @returns the QMetaType typeId of the data type this pin is initialized with */
int CvMatDataInputPin::getTypeId() const
{
    return qMetaTypeId<CvMatData>();
}

/** @returns the name of the type this pin is initialized with */
QString CvMatDataInputPin::getTypeName() const
{
    return QMetaType::typeName( qMetaTypeId<CvMatData>() );
}

CvMatData CvMatDataInputPin::get()
{
    QVariant v;
    getVariant(v);
    if( !v.canConvert<CvMatData>() )
    {
        qWarning("CvMatDataOutputPin expected data with type CvMatData");
    }
    return v.value<CvMatData>();
}

void CvMatDataInputPin::addSupportedDepth(int depth)
{
    m_depths.insert(depth);
}

void CvMatDataInputPin::addSupportedChannels(int channels)
{
    m_channels.insert(channels);
}

void CvMatDataInputPin::removeSupportedDepth(int depth)
{
    m_depths.remove(depth);
}

void CvMatDataInputPin::removeSupportedChannels(int channels)
{
    m_channels.remove(channels);
}

bool CvMatDataInputPin::supportsChannels( int channels ) const
{
    return m_channels.contains(channels);
}

bool CvMatDataInputPin::supportsDepth( int depth ) const
{
    return m_depths.contains(depth);
}

void CvMatDataInputPin::clearDephts()
{
    m_depths.clear();
}

void CvMatDataInputPin::clearChannels()
{
    m_channels.clear();
}

void CvMatDataInputPin::addAllDepths()
{
    m_depths.insert( CV_8U );
    m_depths.insert( CV_8S );
    m_depths.insert( CV_16U );
    m_depths.insert( CV_16S );
    m_depths.insert( CV_32S );
    m_depths.insert( CV_32F );
    m_depths.insert( CV_64F );
}

void CvMatDataInputPin::addAllChannels()
{
    m_channels.insert( 1 );
    m_channels.insert( 2 );
    m_channels.insert( 3 );
    m_channels.insert( 4 );
}


void CvMatDataInputPin::checkImageFormat( const CvMatData& mat )
{
    if( !supportsDepth( mat.getDepth()) )
    {
        QString msg = QString("Depth \"%1\" unsupported by input pin \"%2\"")
                      .arg( CvMatData::depthToString( mat.getDepth() ) )
                      .arg( this->getName() );
        throw PlvRuntimeException( msg, __FILE__, __LINE__ );
    }

    if( !supportsChannels(mat.getChannels()) )
    {
        QString msg = "Number of channels " %
                      QVariant(mat.getChannels()).toString() %
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
bool CvMatDataInputPin::acceptsConnectionWith( const IOutputPin* pin,
                                    QString& errStr ) const
{
    if( pin->getTypeInfo() != this->getTypeInfo() )
    {
        errStr = "Incompatible types between pins";
        return false;
    }
    const CvMatDataOutputPin* ocvOutPin =
            dynamic_cast<const CvMatDataOutputPin*>(pin);

    if( ocvOutPin == 0 )
    {
        errStr = "Expected CvMatData output pin";
        return false;
    }
    return true;
}

/******************** CvMatDataOutputPin ********************************/

const std::type_info& CvMatDataOutputPin::getTypeInfo() const
{
    return typeid( CvMatData );
}

/** @returns the QMetaType typeId of the data type this pin is initialized with */
int CvMatDataOutputPin::getTypeId() const
{
    return qMetaTypeId<CvMatData>();
}

/** @returns the name of the type this pin is initialized with */
QString CvMatDataOutputPin::getTypeName() const
{
    return QMetaType::typeName( qMetaTypeId<CvMatData>() );
}

void CvMatDataOutputPin::put( CvMatData img )
{
    QVariant v;
    v.setValue(img);
    putVariant(v);
}

void CvMatDataOutputPin::addSupportedDepth(int depth)
{
    m_depths.insert( depth );
}

void CvMatDataOutputPin::addSupportedChannels(int channels)
{
    m_channels.insert( channels );
}

void CvMatDataOutputPin::removeSupportedDepth(int depth)
{
    m_depths.remove(depth);
}

void CvMatDataOutputPin::removeSupportedChannels(int channels)
{
    m_channels.remove(channels);
}

bool CvMatDataOutputPin::supportsChannels( int channels ) const
{
    return m_channels.contains(channels);
}

bool CvMatDataOutputPin::supportsDepth( int depth ) const
{
    return m_depths.contains(depth);
}

void CvMatDataOutputPin::clearDephts()
{
    m_depths.clear();
}

void CvMatDataOutputPin::clearChannels()
{
    m_channels.clear();
}

void CvMatDataOutputPin::addAllDepths()
{
    m_depths.insert( CV_8U );
    m_depths.insert( CV_8S );
    m_depths.insert( CV_16U );
    m_depths.insert( CV_16S );
    m_depths.insert( CV_32S );
    m_depths.insert( CV_32F );
    m_depths.insert( CV_64F );
}

void CvMatDataOutputPin::addAllChannels()
{
    m_channels.insert( 1 );
    m_channels.insert( 2 );
    m_channels.insert( 3 );
    m_channels.insert( 4 );
}

void CvMatDataOutputPin::checkImageFormat( const CvMatData& mat )
{
    if( !supportsDepth(mat.getDepth()) )
    {
        QString msg = QString("Depth \"%1\" unsupported by output pin \"%2\"")
                      .arg( CvMatData::depthToString( mat.getDepth() ) )
                      .arg( this->getName() );
        throw PlvRuntimeException( msg, __FILE__, __LINE__ );
    }

    if( !supportsChannels(mat.getChannels()) )
    {
        QString msg = "Number of channels " %
                      QVariant(mat.getChannels()).toString() %
                      " unsupported by output pin \"" % this->getName() %
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
bool CvMatDataOutputPin::acceptsConnectionWith( const IInputPin* pin,
                                    QString& errStr ) const
{
    if( pin->getTypeInfo() != this->getTypeInfo() )
    {
        errStr = "Incompatible types between pins";
        return false;
    }
    const CvMatDataInputPin* ocvInPin =
            dynamic_cast<const CvMatDataInputPin*>(pin);

    if( ocvInPin == 0 )
    {
        errStr = "Expected CvMatData input pin";
        return false;
    }
    return true;
}

CvMatDataOutputPin* plv::createCvMatDataOutputPin( const QString& name, PipelineElement* owner )
throw (IllegalArgumentException)
{
    // if add fails pin is automatically deleted and exception is thrown
    CvMatDataOutputPin* pin = new CvMatDataOutputPin( name, owner );
    owner->addOutputPin( pin );
    return pin;
}

CvMatDataInputPin* plv::createCvMatDataInputPin( const QString& name, PipelineElement* owner,
                                                     IInputPin::InputPinType type )
throw (IllegalArgumentException)
{
    // if add fails pin is automatically deleted and exception is thrown
    CvMatDataInputPin* pin = new CvMatDataInputPin( name, owner, type );
    owner->addInputPin( pin );
    return pin;
}


