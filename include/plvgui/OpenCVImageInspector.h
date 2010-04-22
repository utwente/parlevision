#ifndef OPENCVIMAGEINSPECTOR_H
#define OPENCVIMAGEINSPECTOR_H

#include <QObject>
#include "Inspector.h"

namespace plvgui {
    class OpenCVImageInspector : public QObject, public Inspector
    {
        Q_OBJECT
    };
}
#endif // OPENCVIMAGEINSPECTOR_H
