#ifndef INSPECTORFACTORY_H
#define INSPECTORFACTORY_H

#include <stdexcept>
#include <QString.h>

class QWidget;

namespace plvgui {
    class Inspector;

    class InspectorCreationException : public std::runtime_error
    {
    public:
        InspectorCreationException(std::string msg)
            : std::runtime_error(msg) {}
    };

    class InspectorFactory
    {
    public:
        static Inspector* create(QString dataType, QWidget* parent)
                throw(InspectorCreationException);
    };
}


#endif // INSPECTORFACTORY_H
