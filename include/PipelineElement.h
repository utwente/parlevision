#ifndef PIPELINEELEMENT_H
#define PIPELINEELEMENT_H

#include <map>
#include <stdexcept>
#include <QString>
#include <QObject>
#include <QMetaType>
#include <assert.h>

#include "RefPtr.h"
#include "PlvExceptions.h"

namespace plv
{
    class IInputPin;
    class IOutputPin;
    class Pipeline;

//    typedef enum _PlvPipelineElementState
//    {
//        PLV_PLE_STATE_UNINITIALIZED,
//        PLV_PLE_STATE_NOT_READY,
//        PLV_PLE_STATE_READY
//    } PlvPipelineElementState;

    class PipelineElement : public QObject, public RefCounted
    {
        /** inherits from QObject thus Q_OBJECT macro is necessary */
        Q_OBJECT

        /** typedefs to make code more readable */
        typedef std::map< QString, RefPtr< IInputPin > > InputPinMap;
        typedef std::map< QString, RefPtr< IOutputPin > > OutputPinMap;

    protected:
        /** processor id */
        int m_id;

        /** map which contains the input pins identified and indexed by their name */
        InputPinMap  m_inputPins;

        /** map which contains the output pins identified and indexed by their name */
        OutputPinMap m_outputPins;

    public:
        friend class Pipeline;

        /** QMetaType requires a public default constructor,
         *  a public copy constructor and a public destructor.
         */
        PipelineElement();
        PipelineElement( const PipelineElement& other );
        virtual ~PipelineElement();

        inline void setId( int id ) { assert(m_id == -1); m_id = id; }
        inline int getId() const { return m_id; }

        /** Initialise the element so it is ready to receive
          * process() calls.
          * Should this be reentrant?
          * @return true when initialization succesful
          */
        virtual bool init() = 0;

        /** Adds the input pin to this processing element.
          * @throws IllegalArgumentException if an input pin with
          * the same name already exists
          */
        void addInputPin( IInputPin* pin ) throw (IllegalArgumentException);

        /** Adds the output pin to this processing element.
          * @throws IllegalArgumentException if an input pin with
          * the same name already exists
          */
        void addOutputPin( IOutputPin* pin ) throw (IllegalArgumentException);

        /** @returns the input pin with that name, or null if none exists */
        IInputPin* getInputPin( const QString& name ) const;

        /** @returns the ouput pin with that name, or null if none exists */
        IOutputPin* getOutputPin( const QString& name ) const;

        /** Get a list of properties defined on this element */
        void getConfigurablePropertyNames(std::list<QString>&);

        /** @returns the summed total of all connections in all input pins */
        int inputPinsConnectionCount() const;

        /** @returns the summed total of all connections in all output pins */
        int outputPinsConnectionCount() const;

        /** @returns the summed total of all connections in all input and output pins */
        int pinsConnectionCount() const;

        /** @returns a list of names of input pins added to this PipelineElement */
        std::list<QString>* getInputPinNames() const;

        /** @returns a list of inputpins */
        std::list< RefPtr<IInputPin> >* getInputPins();

        /** @returns a list of outputpins */
        std::list< RefPtr<IOutputPin> >* getOutputPins();

        /** @returns a list of names of output pins added to this PipelineElement */
        std::list<QString>* getOutputPinNames() const;

        /** returns true if there is at least one Pin with a connection */
        bool hasPinConnections() const;

        /** @returns true when this PipelineElement is ready for procesing, which
          * means that the process method is allowed to be called by the scheduler. This
          * method is necessary to support processors which do not require input to be
          * available on all defined pins and hence makes it relatively easy to support
          * asynchronous events using normal pipeline connections. Also, processors could
          * be implemented as state machines, using pipeline connections as change of
          * state signals. For instance, one could design a processor which does A when
          * the light is on, and B when the light is not on, where the light state is
          * connected by a normal processor connection.
          */
        virtual bool isReadyForProcessing() const = 0;

        /** @returns true when bootstrapping of this processor is complete.
          * Some pipelines need to be bootstrapped before they generate valid output.
          * For example, a processor which calculates the history over 5 consecutive
          * frames needs to have seen at least 5 frames for its output to be valid.
          * This method should return true when the requirements which are needed
          * for valid output have been met.
          */
        virtual bool isBootstrapped() const = 0;

        /** This function does the actual work of this PipelineElement and
          * is called by the PipelineScheduler when inputs of this processor
          * are ready i.e. when isReadyForProcessing returns true.
          */
        virtual void process() = 0;

        /** Get the name that describes this element, in human readable form */
        QString getName() const;

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
        static int registerType(QString typeName, QString humanName);

        /** Get a human readable name for the given type
          * @require typeName is a registered type
          * @return a human readable name for the type
          */
        static QString nameForType(QString typeName);

    protected:
        RefPtr<Pipeline> m_parent;

        // list to keep track of registered types
        static std::list<QString> s_types;
        static std::map<QString,QString> s_names;

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
int plvRegisterPipelineElement(const char* typeName, const char* humanName)
{
    plv::PipelineElement::registerType(typeName, humanName);
    return qRegisterMetaType<PET>(typeName);
}


#endif // PIPELINEELEMENT_H
