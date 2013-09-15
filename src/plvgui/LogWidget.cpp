#include "LogWidget.h"

#include <QTextEdit>
#include <QDateTime>
#include <QxtLogger>

using namespace plvgui;

LogWidget::LogWidget(const QString& title, QWidget *parent, int maxLogLength) :
    QDockWidget(title, parent),
    QxtLoggerEngine(),
    m_textEdit(new QTextEdit(this)),
    m_dateFormat("hh:mm:ss.zzz")
{
    this->setWindowTitle("Logger");
    this->setObjectName("LogWindow");
    m_textEdit->setReadOnly(true);
    this->setWidget(m_textEdit);
    qxtLog->addLoggerEngine("plvgui", this);

    m_textEdit->document()->setMaximumBlockCount(maxLogLength);
}

LogWidget::~LogWidget()
{
}

void LogWidget::initLoggerEngine()
{
    enableLogging();
}

void LogWidget::killLoggerEngine()
{
    disableLogging();
}

bool LogWidget::isInitialized() const
{
    return true;
}

void LogWidget::writeFormatted(QxtLogger::LogLevel level, const QList<QVariant>& messages)
{
    if(messages.isEmpty()) return;

    QString levelString = QxtLogger::logLevelToString(level);
    QString message = '[' + QDateTime::currentDateTime().toString(m_dateFormat) + "] [" + levelString + "] ";

    foreach( const QVariant& out, messages )
    {
        if( !out.isNull() )
        {
            message.append(out.toString().toUtf8());
            message.append(" ");
        }
    }
    m_textEdit->append(message);
}
