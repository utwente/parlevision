#include "openeventhandler.h"
#include <QFileOpenEvent>

#include <MainWindow.h>

using namespace plvgui;

OpenEventHandler::OpenEventHandler(QObject *parent) :
    QObject(parent)
{
}

bool OpenEventHandler::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::FileOpen)
    {
        MainWindow* mainWin = new MainWindow();
        mainWin->show();
        mainWin->loadFile(static_cast<QFileOpenEvent *>(event)->file());
        return true;
    }

    return QObject::eventFilter(obj, event);
}
