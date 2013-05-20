#ifndef PIPELINEELEMENTFACTORY_H
#define PIPELINEELEMENTFACTORY_H

#include <QString>
#include <QHash>
#include <QDebug>

#include "plvglobal.h"
#include "PlvExceptions.h"

namespace plv
{
    class PipelineElement;

    class PLVCORE_EXPORT PipelineElementConstructor
    {
    public:
        virtual PipelineElement* construct() = 0;
        virtual const char* getClassName() const = 0;
        virtual ~PipelineElementConstructor() {}
    };

    class PLVCORE_EXPORT PipelineElementFactory
    {
    public:
        /** returns element id or -1 on failure */
        static int registerElement( PipelineElementConstructor* constructor );

        /** unregisters the element constructor */
        static void unregisterElement( const QString& name );

        /** returns the PipelineElements id if element with name 'name' is
            registered. Returns -1 if not registered. */
        static int elementId( const QString& name );

        /** returns a new instance of the PipelineElement with name 'name'.
            returns NULL when there is no PipelineElement with name 'name'. */
        static PipelineElement* construct( const QString& name );

        /** returns a new instance of the PipelineElement with id 'id'.
            returns NULL when the id is invalid. Very fast, constant time. */
        static PipelineElement* construct( int id );

        /** Get a list of all known PipelineElement Type names */
        static QStringList types();

    protected:
        static QList< PipelineElementConstructor* > m_elementConstructors;
        static QHash<QString, int> m_nameIdMapping;
    };

    template< class T >
    class PipelineElementConstructorHelper : public PipelineElementConstructor
    {
    public:
        virtual PipelineElement* construct()
        {
            return static_cast<PipelineElement*>(new T);
        }

        virtual const char* getClassName() const
        {
            return (reinterpret_cast<T*>(0))->staticMetaObject.className();
        }
    };
}

/** template helper function to register PipelineElements */
template<typename PET>
int plvRegisterPipelineElement()
{
    plv::PipelineElementConstructorHelper<PET>* plec = new plv::PipelineElementConstructorHelper<PET>();
    int id = plv::PipelineElementFactory::registerElement( plec );
    return id;
}

#endif
