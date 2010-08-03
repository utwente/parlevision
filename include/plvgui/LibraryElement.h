#ifndef LIBRARYELEMENT_H
#define LIBRARYELEMENT_H

#include <QString>
#include <QWidget>
#include <QHBoxLayout>
#include "RefPtr.h"
#include "PipelineElement.h"

class QVBoxLayout;
class QHBoxLayout;

namespace plv
{
//    class PipelineElement;
}

namespace plvgui {
    class LibraryElement : public QWidget
    {
        Q_OBJECT

    public:
        LibraryElement(plv::RefPtr<plv::PipelineElement> element, QWidget* parent);
        plv::RefPtr<plv::PipelineElement> getElement() { return element; }

    signals:
        void pressed();
        void moved();
        void released();

    protected:
        virtual QSize sizeHint() const;
        virtual int heightForWidth(int w) const;
        void mousePressEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void paintEvent(QPaintEvent*);

    private:
        plv::RefPtr<plv::PipelineElement> element;

        QVBoxLayout* outerContainer;
        QHBoxLayout* innerContainer;
        QVBoxLayout* inPinContainer;
        QVBoxLayout* outPinContainer;
    };
}

#endif // LIBRARYELEMENT_H
