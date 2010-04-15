#ifndef PIPELINEELEMENT_H
#define PIPELINEELEMENT_H

#include <map>
#include <QString>
#include <QObject>

#include "RefPtr.h"

namespace plv
{
    class Pin;
    class Pipeline;

    typedef enum _PlvPipelineElementState
    {
        PROCESSOR_OK,
        PROCESSOR_CONFIG_NEEDED
    } PlvPipelineElementState;

    class PipelineElement : public QObject, public RefCounted
    {
        Q_OBJECT

        typedef std::map< QString, RefPtr< Pin > > InputPinMap;
        typedef std::map< QString, RefPtr< Pin > > OutputPinMap;

    protected:
        InputPinMap  m_inputPins;
        OutputPinMap m_outputPins;

    public:
        PipelineElement( Pipeline* parent );

        virtual PlvPipelineElementState init() = 0;
        virtual PlvPipelineElementState checkConfig() = 0;

        void addOutputPin ( Pin* pin );

        const Pin* getInputPin( const QString& name ) const;
        const Pin* getOutputPin( const QString& name ) const;

    protected:
        virtual ~PipelineElement();

        RefPtr<Pipeline> m_parent;
    };
}

#endif // PIPELINEELEMENT_H
