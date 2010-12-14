#ifndef CVMATDATAOUTPUTPIN_H
#define CVMATDATAOUTPUTPIN_H

#include "Pin.h"
#include "CvMatData.h"

namespace plv
{
    class CvMatDataOutputPin;
    class PipelineElement;

    class PLVCORE_EXPORT CvMatDataInputPin : public IInputPin
    {
    public:
        CvMatDataInputPin( const QString& name,
                           PipelineElement* owner,
                           InputPinType type = CONNECTION_REQUIRED ) :
                           IInputPin( name, owner, type )
        {
        }

        CvMatData get();

        void addSupportedDepth(int depth);
        void addSupportedChannels(int channels);

        void removeSupportedDepth(int depth);
        void removeSupportedChannels(int channels);

        bool supportsChannels( int channels ) const;
        bool supportsDepth( int depth ) const;

        void clearDephts();
        void clearChannels();

        void addAllDepths();
        void addAllChannels();

        /** Done at runtime */
        void checkImageFormat( const CvMatData& img );

        /** Done once at connection creation only */
        virtual bool acceptsConnectionWith(const IOutputPin* pin, QString& errStr) const;

        virtual const std::type_info& getTypeInfo() const;
        virtual int getTypeId() const;
        virtual QString getTypeName() const;

    private:
        QSet<int> m_depths;
        QSet<int> m_channels;
    };

    class PLVCORE_EXPORT CvMatDataOutputPin : public IOutputPin
    {
    public:
        CvMatDataOutputPin( const QString& name, PipelineElement* owner ) :
                IOutputPin( name, owner ) {}

        /** Puts data in connection. Drops data if no connection present. */
        void put( CvMatData img );

        void addSupportedDepth(int depth);
        void addSupportedChannels(int channels);
        void removeSupportedDepth(int depth);
        void removeSupportedChannels(int channels);
        bool supportsChannels( int channels ) const;
        bool supportsDepth( int depth ) const;

        void clearDephts();
        void clearChannels();

        void addAllDepths();
        void addAllChannels();

        /** Done at runtime */
        void checkImageFormat( const CvMatData& img );

        /** Done once at connection creation only */
        virtual bool acceptsConnectionWith(const IInputPin* pin, QString& errStr) const;

        virtual const std::type_info& getTypeInfo() const;
        virtual int getTypeId() const;
        virtual QString getTypeName() const;

    protected:
        QSet<int> m_depths;
        QSet<int> m_channels;
    };

    CvMatDataOutputPin* PLVCORE_EXPORT createCvMatDataOutputPin( const QString& name, PipelineElement* owner )
    throw (IllegalArgumentException);

    CvMatDataInputPin* PLVCORE_EXPORT createCvMatDataInputPin( const QString& name, PipelineElement* owner,
                                  IInputPin::InputPinType type = IInputPin::CONNECTION_REQUIRED )
    throw (IllegalArgumentException);
}

#endif // CvMatDataOUTPUTPIN_H
