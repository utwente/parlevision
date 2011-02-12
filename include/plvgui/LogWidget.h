#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QDockWidget>
#include <QxtLoggerEngine>

QT_FORWARD_DECLARE_CLASS(QTextEdit);

namespace plvgui
{

    class LogWidget : public QDockWidget, public QxtLoggerEngine
    {
        Q_OBJECT
    public:
        explicit LogWidget(const QString& title, QWidget *parent = 0);

        virtual void initLoggerEngine();
        virtual void killLoggerEngine();
        virtual bool isInitialized() const;
        virtual void writeFormatted(QxtLogger::LogLevel level, const QList<QVariant>& messages);

    signals:

    public slots:

    private:
        QTextEdit* m_textEdit;
        QString m_dateFormat;
    };

}

#endif // LOGWIDGET_H
