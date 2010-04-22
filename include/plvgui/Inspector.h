#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <QWidget>

namespace plvgui {

    /** Interface for Inspectors that observe or monitor pins of a certain dataType.
      * Each implementation must register itself with the InspectorFactory
      * and implement the methods needed to render the datatype.
      */
    class Inspector
    {

    };
}

#endif // INSPECTOR_H
