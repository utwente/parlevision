#ifndef OPENCVIMAGEOUTPUTPIN_H
#define OPENCVIMAGEOUTPUTPIN_H

#include "Pin.h"
#include "OpenCVImage.h"

namespace plv
{
    /** functor base class which can be implemented for compatibility callbacks */
//    class PLVCORE_EXPORT OpenCVImageDataCompatibilityCallback :
//            public DataCompatibilityCallback
//    {
//    };

    class OpenCVImageOutputPin;

    class PLVCORE_EXPORT OpenCVImageInputPin : public IInputPin
    {
    public:
        OpenCVImageInputPin( const QString& name,
                             PipelineElement* owner,
                             InputPinType type = INPUT_REQUIRED ) :
                             IInputPin( name, owner, type )
        {
        }

        inline RefPtr<OpenCVImage> get() throw (PlvRuntimeException)
        {
            // We can safely do this because this Pin is
            // guaranteed to be connected to a Pin of the
            // same type.
            RefPtr<Data> data;
            this->getUntyped( data );
#ifdef DEBUG
            RefPtr<OpenCVImage> img = ref_ptr_dynamic_cast<OpenCVImage>(data);
            assert(img.isNotNull());
#else
            RefPtr<OpenCVImage> img = ref_ptr_static_cast<OpenCVImage>(data);
#endif
            checkImageFormat( img );

            return img;
        }

        virtual const std::type_info& getTypeInfo() const;

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
        void checkImageFormat( const RefPtr<OpenCVImage>& img )
                throw (PlvRuntimeException);

        /** Done once at connection creation only */
        virtual bool acceptsConnectionWith(const IOutputPin* pin, QString& errStr) const;

    protected:
        QSet<int> m_depths;
        QSet<int> m_channels;
    };

    class PLVCORE_EXPORT OpenCVImageOutputPin : public IOutputPin
    {
    public:
        OpenCVImageOutputPin( const QString& name, PipelineElement* owner ) :
                IOutputPin( name, owner ) {}

        /** Puts data in connection. Drops data if no connection present.
          * Throws an exception if the data violates format contract,
          * which is checked in the callback functor of IOutputPin
          */
        inline void put( const RefPtr<OpenCVImage>& img )
        {
            checkImageFormat( img );
            Data* data = static_cast<Data*>( img.getPtr() );
            this->putUntyped( data );
        }

        virtual const std::type_info& getTypeInfo() const;
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
        void checkImageFormat( const RefPtr<OpenCVImage>& img )
                throw (PlvRuntimeException);

        /** Done once at connection creation only */
        virtual bool acceptsConnectionWith(const IInputPin* pin, QString& errStr) const;

    protected:
        QSet<int> m_depths;
        QSet<int> m_channels;
    };

    OpenCVImageOutputPin* PLVCORE_EXPORT createOpenCVImageOutputPin( const QString& name, PipelineElement* owner )
    throw (IllegalArgumentException);

    OpenCVImageInputPin* PLVCORE_EXPORT createOpenCVImageInputPin( const QString& name, PipelineElement* owner,
                                  IInputPin::InputPinType type = IInputPin::INPUT_REQUIRED )
    throw (IllegalArgumentException);
}

#endif // OPENCVIMAGEOUTPUTPIN_H
