#ifndef OPENCVIMAGEINSPECTOR_H
#define OPENCVIMAGEINSPECTOR_H

#include <QObject>
#include "Inspector.h"

namespace plv {
    class Data;
}

namespace plvgui {
    class OpenCVImageInspector : public Inspector
    {
        virtual void newData(Data* data);
    };
}
#endif // OPENCVIMAGEINSPECTOR_H
