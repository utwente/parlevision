#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <QWidget>
#include <QObject>

namespace plv {
    class Data;
    class Pin;
}

using namespace plv;

namespace plvgui {

    /** Interface for Inspectors that observe or monitor pins of a certain dataType.
      * Each implementation must register itself with the InspectorFactory
      * and implement the methods needed to render the datatype.
      */
    class Inspector : public QObject
    {
        Q_OBJECT

    public:
        ~Inspector();

        /** Start inspecting this pin
          * Disconnects all previous connections
          */
        virtual void setPin(const plv::Pin* pin);

    public slots:
        virtual void newData(Data* data)=0;

    };
}

#endif // INSPECTOR_H
