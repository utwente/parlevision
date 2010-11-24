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

#ifndef RENDERERFACTORY_H
#define RENDERERFACTORY_H

#include <stdexcept>
#include <typeinfo>
#include <QString>
#include <QHash>
#include <plvcore/RefCounted.h>
#include <plvcore/RefPtr.h>
#include "DataRenderer.h"

class QWidget;

namespace plvgui
{
    // abstract base class for constructors
    class DataRendererConstructor : public plv::RefCounted
    {
    public:
        virtual const char* getDataTypeName() const = 0;
        virtual DataRenderer* create(QWidget* parent) const = 0;
    };

    template< class DT, class RT >
    class DataRendererConstructorHelper : public DataRendererConstructor
    {
    public:
        const char* getDataTypeName() const
        {
            const std::type_info& info = typeid( DT );
            return info.name();
        }

        DataRenderer* create(QWidget* parent) const
        {
            DataRenderer* r = static_cast<DataRenderer*>(new RT(parent));
            r->setParent( parent );
            return r;
        }
    };

    class RendererFactory
    {
    protected:
        static RendererFactory* m_instance;
        QHash<QString, plv::RefPtr<DataRendererConstructor> > m_rendererConstructors;

        void add( DataRendererConstructor* constructor );

    public:
        inline static RendererFactory* instance()
        {
            if(m_instance==0)
                m_instance = new RendererFactory();
            return m_instance;
        }

        // usage plvgui::RendererFactory::add<MyDataRenderer>();
        template< class DT, class RT >
        static void add()
        {
            DataRendererConstructor* c = new DataRendererConstructorHelper<DT, RT>();
            instance()->add( c );
        }

        DataRenderer* create( QString dataType, QWidget* parent );
    };
}


#endif // RENDERERFACTORY_H
