#ifndef LIBRARYELEMENT_H
#define LIBRARYELEMENT_H

#include <QString>
#include <QLabel>

class QWidget;


namespace plvgui {
    class LibraryElement : public QLabel
    {
    public:
        LibraryElement(QString name, QWidget* parent);
    };
}

#endif // LIBRARYELEMENT_H
