/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvgui module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#ifndef INSPECTORWIDGET_H
#define INSPECTORWIDGET_H

#include <QtGui/QDockWidget>

#include <plvcore/RefPtr.h>
#include <plvcore/PipelineElement.h>
#include <plvcore/Types.h>

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
        virtual ~InspectorWidget();

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
