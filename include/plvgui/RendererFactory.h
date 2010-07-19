#ifndef RENDERERFACTORY_H
#define RENDERERFACTORY_H

#include <stdexcept>
#include <QString.h>

class QWidget;

namespace plvgui
{
    class DataRenderer;

    class RendererCreationException : public std::runtime_error
    {
    public:
        RendererCreationException(std::string msg)
            : std::runtime_error(msg) {}
    };

    class RendererFactory
    {
    public:
        static DataRenderer* create(QString dataType, QWidget* parent)
                throw(RendererCreationException);
    };
}


#endif // RENDERERFACTORY_H
