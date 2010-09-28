/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
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

#include "Types.h"

using namespace plv;

Enum::Enum( int selected )
{
    m_selectedIndex = selected;
}

Enum::~Enum()
{
}

Enum::Enum( const Enum& other )
{
    m_selectedIndex = other.m_selectedIndex;
    m_items = other.m_items;
}

void Enum::setSelectedIndex( int i )
{
    assert( m_items.contains( i ) );
    m_selectedIndex = i;
}

void Enum::setSelected( const QString& selected )
{
    int i = 0;
    foreach( const EnumPair& p , m_items.values() )
    {
        if( p.name().compare( selected ) == 0 )
        {
            m_selectedIndex = i;
            break;
        }
        ++i;
    }
}

void Enum::add( const QString& str )
{
    m_items.insert( m_items.size(), EnumPair( str, m_items.size() ));
}

void Enum::add( const QString& str, int value )
{
    m_items.insert( m_items.size(), EnumPair( str,value ));
}

int Enum::getSelectedIndex() const
{
    return m_selectedIndex;
}

QString Enum::getSelectedItemName() const
{
    return getItemName( getSelectedIndex() );
}

int Enum::getSelectedValue() const
{
    return getItemValue( m_selectedIndex );
}

QString Enum::getItemName( int i ) const
{
    if( m_items.contains(i) )
        return m_items.value(i).name();
    else
        return "INVALID";
}

int Enum::getItemValue( int i ) const
{
    if( m_items.contains(i) )
        return m_items.value(i).value();
    else
        return -1;
}

QStringList Enum::getItemNames() const
{
    QStringList list;

    for( int i=0; i < m_items.size(); ++i )
    {
        list.append( m_items.value(i).name() );
    }

    return list;
}

QString Enum::toString() const
{
    QString str = "";
    for( int i=0; i < m_items.size(); ++i )
    {
        if( i == m_selectedIndex )
            str+= " => ";
        else
            str+= "    ";

        str += m_items.value(i).name() + " " +
               m_items.value(i).value() + "\n";
    }
    return str;
}
