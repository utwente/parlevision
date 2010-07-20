#ifndef DATARENDERER_H
#define DATARENDERER_H

#include <QWidget>
#include <QObject>

#include "RefPtr.h"
#include "Types.h"

namespace plv
{
    class Pin;
}

using namespace plv;

namespace plvgui {

    /** Interface for DataRenderers that observe or monitor pins of a certain dataType.
      * Each implementation must register itself with the DataRendererFactory
      * and implement the methods needed to render the datatype.
      */
    class DataRenderer : public QWidget
    {
        Q_OBJECT

    public:
        DataRenderer( QWidget* parent );
        virtual ~DataRenderer();

        /** Start inspecting this pin
          * Disconnects all previous connections
          */
        virtual void setPin(const plv::Pin* pin);

    public slots:
        virtual void newData( RefPtr<Data> data );

    };
}

#endif // DATARENDERER_H
