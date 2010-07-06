#ifndef PIPELINEELEMENT_H
#define PIPELINEELEMENT_H

#include <map>
#include <stdexcept>
#include <QString>
#include <QObject>
#include <QMetaType>

#include "RefPtr.h"

namespace plv
{
    class IInputPin;
    class IOutputPin;
    class Pipeline;

    typedef enum _PlvPipelineElementState
    {
        PLV_PLE_STATE_UNINITIALIZED,
        PLV_PLE_STATE_NOT_READY,
        PLV_PLE_STATE_READY
    } PlvPipelineElementState;

    class ElementCreationException : public std::runtime_error
    {
    public:
        ElementCreationException(std::string msg)
            : std::runtime_error(msg) {}
    };

    class PipelineElement : public QObject, public RefCounted
    {
        Q_OBJECT

        typedef std::map< QString, RefPtr< IInputPin > > InputPinMap;
        typedef std::map< QString, RefPtr< IOutputPin > > OutputPinMap;

    protected:
        InputPinMap  m_inputPins;
        OutputPinMap m_outputPins;

    public:
        friend class Pipeline;

        /** QMetaType requires a public default constructor,
         *  a public copy constructor and a public destructor.
         */
        PipelineElement();
        PipelineElement( const PipelineElement& other );
        virtual ~PipelineElement();

        /** Initialise the element so it is ready to receive
          * process() calls.
          * Should this be reentrant?
          */
        virtual PlvPipelineElementState init() = 0;
        virtual PlvPipelineElementState checkConfig() = 0;

        void addInputPin( IInputPin* pin );
        void addOutputPin( IOutputPin* pin );

        IInputPin* getInputPin( const QString& name ) const;
        IOutputPin* getOutputPin( const QString& name ) const;

        /** This function does the actual work of this PipelineElement and
          * is called by the PipelineScheduler when inputs of this processor
          * are ready.
          */
        virtual void process() = 0;



        /** Get a list of all known PipelineElement Type names
          */
        static std::list<QString> types();

        /** Register the given type as a PipelineElement Type.
          * The type needs to be known to Qt's MetaType system,
          * so you will likely rarely call this yourself.
          * Use one of the plvRegisterPipelineElement macros instead.
          * @require typeName was not registered to PipelineElement before.
          * @require typeName is a type registered to the Qt MetaType system
          *     e.g. QMetaType::type(typeName) returns a valid ID
          */
        static int registerType(QString typeName);


    protected:
        RefPtr<Pipeline> m_parent;

        // list to keep track of registered types
        static std::list<QString> s_types;

        /**
         * This gets called by Pipeline when we are added to it.
         * Handles removing ourself from any previous pipeline we were part of
         * and sets m_parent to the new pipeline
         */
        virtual void setPipeline(Pipeline* parent);

        /**
          * private process function which handles scoping of input and output pins
          * and class the process() function of the super class.
          */
        void __process();
    };
}

template<typename PET>
int plvRegisterPipelineElement(const char* typeName)
{
    plv::PipelineElement::registerType(typeName);
    return qRegisterMetaType<PET>(typeName);
}


#endif // PIPELINEELEMENT_H
