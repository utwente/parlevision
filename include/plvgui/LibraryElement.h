#ifndef LIBRARYELEMENT_H
#define LIBRARYELEMENT_H

#include <QString>
#include <QWidget>
#include <QHBoxLayout>
#include "RefPtr.h"

class QVBoxLayout;
class QHBoxLayout;

namespace plv
{
    class PipelineElement;
}

namespace plvgui {
    class LibraryElement : public QWidget
    {
    public:
        LibraryElement(plv::RefPtr<plv::PipelineElement> element, QWidget* parent);
        plv::RefPtr<plv::PipelineElement> getElement() { return element; }

    protected:
        virtual QSize sizeHint() const;
        virtual int heightForWidth(int w) const;

    private:
        plv::RefPtr<plv::PipelineElement> element;

        QVBoxLayout* outerContainer;
        QHBoxLayout* innerContainer;
        QVBoxLayout* inPinContainer;
        QVBoxLayout* outPinContainer;
    };
}

#endif // LIBRARYELEMENT_H
