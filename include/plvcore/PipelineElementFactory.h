#ifndef PIPELINEELEMENTFACTORY_H
#define PIPELINEELEMENTFACTORY_H

#include <QString>
#include <QHash>

#include "plvglobal.h"
#include "PlvExceptions.h"

namespace plv
{
    class PipelineElement;
    class IllegalArgumentException;

    class PLVCORE_EXPORT PipelineElementConstructor
    {
    public:
        virtual PipelineElement* construct() = 0;
        virtual QString getName() const = 0;
    };

    class PLVCORE_EXPORT PipelineElementFactory
    {
    protected:
        /** constructor. Protected because it is a singleton */
        PipelineElementFactory();

        /** destructor. Protected, use clear instead */
        ~PipelineElementFactory();

        /** returns element id or -1 on failure */
        int __registerElement( PipelineElementConstructor* constructor );

        /** unregisters the element constructor */
        void __unregisterElement( const QString& name );

        /** returns the PipelineElements id if element with name 'name' is
            registered. Returns -1 if not registered. */
        int __isElementRegistered( const QString& name );

        /** returns a new instance of the PipelineElement with name 'name' */
        PipelineElement* __construct( const QString& name ) const;

        PipelineElement* __construct( int id ) const;

    public:
        /** returns element id or -1 on failure */
        inline static int registerElement( PipelineElementConstructor* constructor )
        {
            return PipelineElementFactory::instance()->__registerElement(constructor);
        }

        /** unregisters the element constructor */
        inline static void unregisterElement( const QString& name )
        {
            return PipelineElementFactory::instance()->__unregisterElement(name);
        }

        /** returns the PipelineElements id if element with name 'name' is
            registered. Returns -1 if not registered. */
        inline static int isElementRegistered( const QString& name )
        {
            return PipelineElementFactory::instance()->__isElementRegistered(name);
        }

        /** returns a new instance of the PipelineElement with name 'name' */
        inline static PipelineElement* construct( const QString& name )
        {
            return PipelineElementFactory::instance()->__construct(name);
        }

        inline static PipelineElement* construct( int id )
        {
            return PipelineElementFactory::instance()->__construct(id);
        }

        /** returns an instance of this factory */
        inline static PipelineElementFactory* instance()
        {
            if( m_instance == 0 )
                m_instance = new PipelineElementFactory();

            return m_instance;
        }

        /** static destructor */
        inline static void clear()
        {
            if(m_instance != 0)
            {
                delete m_instance;
                m_instance = 0;
            }
        }

    protected:
        static PipelineElementFactory* m_instance;
        QList<PipelineElementConstructor*> m_elementConstructors;
        QHash<QString, int> m_nameIdMapping;
    };

    template< class T >
    class PipelineElementConstructorHelper : public PipelineElementConstructor
    {
    public:
        PipelineElementConstructorHelper( const QString& name ) :
            m_name( name )
        {
        }

        virtual PipelineElement* construct()
        {
            return static_cast<PipelineElement*>(new T);
        }

        QString getName() const { return m_name; }

    protected:
        QString m_name;
    };
}
#endif
