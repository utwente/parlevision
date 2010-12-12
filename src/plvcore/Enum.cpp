#include "Enum.h"

#include <QStringList>

using namespace plv;

Enum::Enum( int selected ) : m_selectedIndex( selected ) {}
Enum::~Enum() {}

Enum::Enum( const Enum& other ) :
    m_selectedIndex( other.m_selectedIndex ),
    m_items( other.m_items )
{
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

void Enum::add( const QString& str, int value )
{
    m_items.insert( m_items.size(), EnumPair( str,value ));
}

void Enum::add( const QString& str )
{
    m_items.insert( m_items.size(), EnumPair( str, m_items.size() ));
}

int Enum::getSelectedIndex() const
{
    return m_selectedIndex;
}

QString Enum::getSelectedItemName() const throw (PlvRuntimeException)
{
    return getItemName( getSelectedIndex() );
}

int Enum::getSelectedValue() const throw (PlvRuntimeException)
{
    return getItemValue( m_selectedIndex );
}

QString Enum::getItemName( int i ) const throw (PlvRuntimeException)
{
    if( m_items.contains(i) )
    {
        return m_items.value(i).name();
    }
    else
    {
        QString msg( "Invalid index" );
        throw PlvRuntimeException( msg, __FILE__, __LINE__ );
    }
}

int Enum::getItemValue( int i ) const
        throw (PlvRuntimeException)
{
    if( m_items.contains(i) )
    {
        return m_items.value(i).value();
    }
    else
    {
        QString msg( "Invalid index" );
        throw PlvRuntimeException( msg, __FILE__, __LINE__ );
    }
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
