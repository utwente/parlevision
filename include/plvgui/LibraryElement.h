#ifndef LIBRARYELEMENT_H
#define LIBRARYELEMENT_H

#include <QString>
#include <QLabel>
#include <QHBoxLayout>

namespace plvgui {
    class LibraryElement : public QLabel
    {
    public:
        LibraryElement(QString typeName, QWidget* parent);
        QString getType() { return typeName; }
        QString getName() { return name; }

    protected:
        virtual QSize sizeHint() const;
        virtual int heightForWidth(int w) const;

    private:
        QString typeName;
        QString name;
    };
}

#endif // LIBRARYELEMENT_H
