#ifndef PIN_H
#define PIN_H

#include <assert.h>
#include <typeinfo>
#include <QImage>

#include "RefPtr.h"
#include "RefCounted.h"
#include "PinConnection.h"
#include "PipelineElement.h"

namespace plv {
    class Pin : public RefCounted
    {
    public:
        typedef enum _PinType {
            CONSUMER,
            PRODUCER
        } PinType;

        Pin( const QString& name, PipelineElement* owner ) :
            m_name( name ),
            m_owner( owner )
        {
            assert( m_owner.isValid() );
        }

        const QString& getName() { return m_name; }

    protected:
         /** the name of this Pin e.g. "BlackAndWhite" */
        QString m_name;

        /** a unique id of this Pin */
        int m_id;

        /** either PRODUCER or CONSUMER */
        PinType m_type;

        ///** the pin data type. See Types.h for details */
        //PinDataType m_dataType;

        /** data received from or delivered to this pipeline element */
        RefPtr<PipelineElement> m_owner;

        /** null if there is no connection */
        RefPtr<PinConnection>   m_connection;

        ~Pin() {};

    public:

        inline int getId() const { return m_id; }
        inline PipelineElement* getOwner() const { return m_owner.getPtr(); }
        inline PinConnection* getPinConnection() const { return m_connection.getPtr(); }
        //inline PinType getType() const { return m_type; }

        virtual const std::type_info& getType() const =0;
    };

    template< class T >
    class TypedPin : public Pin
    {
    public:
        TypedPin( const QString& name, PipelineElement* owner ) :
                Pin( name, owner ) {}

        virtual const std::type_info& getType() const
        {
            return typeid( T );
        }
    };

}

#endif // PIN_H
