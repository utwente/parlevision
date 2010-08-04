#ifndef INSPECTORWIDGET_H
#define INSPECTORWIDGET_H

#include <QDockWidget>

#include "RefPtr.h"
#include "PipelineElement.h"
#include "Types.h"

class QFormLayout;

namespace Ui
{
    class InspectorWidget;
}

namespace plv
{
//    class PipelineElement;
}

using namespace plv;

namespace plvgui
{
    class PlvEnumProxy : public QObject
    {
        Q_OBJECT
    protected:
        plv::Enum m_enum;

    public:
        PlvEnumProxy( plv::Enum e ) : m_enum( e )
        {
        }

    signals:
        void indexSet( int );
        void indexSet( plv::Enum );

    public slots:
        void enumToInt( plv::Enum e )
        {
            int index = e.getSelectedIndex();
            emit( indexSet( index ));
        }

        void intToEnum( int i )
        {
            m_enum.setSelectedIndex( i );
            emit( indexSet( m_enum ));
        }
    };

    class InspectorWidget : public QDockWidget
    {
        Q_OBJECT

    public:
        explicit InspectorWidget(QWidget *parent = 0);
        ~InspectorWidget();

    public slots:
        /** set the currently selected target to this one
          * this automatically unselects any previous selections.
          */
        void setTarget(plv::RefPtr<plv::PipelineElement> element);
        void nothingSelected();
        void multipleSelected();

    protected:
        void changeEvent(QEvent *e);

    private:
        void clearSelection();
        void addRow(QFormLayout* form, RefPtr<PipelineElement> element, const QString& name, int value);
        void addRow(QFormLayout* form, RefPtr<PipelineElement> element, const QString& name, double value);
        void addRow(QFormLayout* form, RefPtr<PipelineElement> element, const QString& name, bool value);
        void addRow(QFormLayout* form, RefPtr<PipelineElement> element, const QString& name, const QString& value, bool editable=true);
        //void addRow(QFormLayout* form, RefPtr<PipelineElement> element, const QString& name, QMetaEnum metaEnum );
        void addRow(QFormLayout* form, RefPtr<PipelineElement> element, const QString& name, plv::Enum plvEnum );
        Ui::InspectorWidget *ui;
        RefPtr<PipelineElement> element;
        QWidget* formContainer;

        QString propertySlotSignature(QObject* obj, QString property, QString signature = "");
        QMap<QString, PlvEnumProxy*> m_enumProxies;
    };
}
#endif // INSPECTORWIDGET_H
