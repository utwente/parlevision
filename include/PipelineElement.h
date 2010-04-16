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
        PLV_PLE_STATE_UNINITIALIZED,
        PLV_PLE_STATE_NOT_READY,
        PLV_PLE_STATE_READY
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

        void addInputPin( Pin* pin );
        void addOutputPin( Pin* pin );

        const Pin* getInputPin( const QString& name ) const;
        const Pin* getOutputPin( const QString& name ) const;

    protected:
        /** pipeline elements cannot be copied */
        PipelineElement( const PipelineElement& other );

        /** RefCounted classes have protected destructors */
        virtual ~PipelineElement();

        RefPtr<Pipeline> m_parent;
    };
}

#endif // PIPELINEELEMENT_H
