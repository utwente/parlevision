#ifndef OPENEVENTHANDLER_H
#define OPENEVENTHANDLER_H

#include <QObject>

class OpenEventHandler : public QObject
{
    Q_OBJECT
public:
    explicit OpenEventHandler(QObject *parent = 0);

protected:
    bool eventFilter(QObject* obj, QEvent *event);

signals:

public slots:

};

#endif // OPENEVENTHANDLER_H
