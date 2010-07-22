#ifndef LIBRARYELEMENT_H
#define LIBRARYELEMENT_H

#include <QString>
#include <QWidget>
#include <QHBoxLayout>

class QWidget;


namespace plvgui {
    class LibraryElement : public QWidget
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
        QVBoxLayout* layout;
    };
}

#endif // LIBRARYELEMENT_H
