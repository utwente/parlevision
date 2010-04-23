#ifndef PIPELINEELEMENT_H
#define PIPELINEELEMENT_H

#include <map>
#include <QString>
#include <QObject>

#include "RefPtr.h"

namespace plv
{
    class InputPin;
    class OutputPin;
    class Pipeline;

    typedef enum _PlvPipelineElementState
    {
        PLV_PLE_STATE_UNINITIALIZED,
        PLV_PLE_STATE_NOT_READY,
        PLV_PLE_STATE_READY
    } PlvPipelineElementState;

    class PipelineElement : public QObject, public RefCounted
    {
        Q_OBJECT

        typedef std::map< QString, RefPtr< InputPin > > InputPinMap;
        typedef std::map< QString, RefPtr< OutputPin > > OutputPinMap;

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

        virtual PlvPipelineElementState init() = 0;
        virtual PlvPipelineElementState checkConfig() = 0;

        void addInputPin( InputPin* pin );
        void addOutputPin( OutputPin* pin );

        const InputPin* getInputPin( const QString& name ) const;
        const OutputPin* getOutputPin( const QString& name ) const;

    protected:
        RefPtr<Pipeline> m_parent;
        /**
         * This gets called by Pipeline when we are added to it.
         * Handles removing ourself from any previous pipeline we were part of
         * and sets m_parent to the new pipeline
         */
        virtual void setPipeline(Pipeline* parent);
    };
}

#endif // PIPELINEELEMENT_H
