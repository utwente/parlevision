#ifndef LIBRARYELEMENT_H
#define LIBRARYELEMENT_H

#include <QString>
#include <QLabel>

class QWidget;


namespace plvgui {
    class LibraryElement : public QLabel
    {
    public:
        LibraryElement(QString typeName, QWidget* parent);
        QString getType() { return typeName; }
        QString getName() { return name; }

    private:
        QString typeName;
        QString name;
    };
}

#endif // LIBRARYELEMENT_H
