#ifndef LIBRARYELEMENT_H
#define LIBRARYELEMENT_H

#include <QString>
#include <QLabel>
#include <QHBoxLayout>
#include "RefPtr.h"

namespace plv
{
    class PipelineElement;
}

namespace plvgui {
    class LibraryElement : public QLabel
    {
    public:
        LibraryElement(plv::RefPtr<plv::PipelineElement> element, QWidget* parent);
        plv::RefPtr<plv::PipelineElement> getElement() { return element; }

    protected:
        virtual QSize sizeHint() const;
        virtual int heightForWidth(int w) const;

    private:
        plv::RefPtr<plv::PipelineElement> element;
    };
}

#endif // LIBRARYELEMENT_H
