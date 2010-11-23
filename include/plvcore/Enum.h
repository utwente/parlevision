/** private class used to store enum information in Enum class */
#ifndef PLVENUM_H
#define PLVENUM_H

#include "plvglobal.h"

#include <QString>
#include <QMap>

#include <QMetaType>

#include <assert.h>

#include "PlvExceptions.h"

namespace plv
{
    class EnumPair;

    /** Class for selectable enum properties with string represenation of name.
      * Useful in GUI code
      */
    class PLVCORE_EXPORT Enum
    {
    public:
        Enum( int selected = 0 );

        ~Enum();

        Enum( const Enum& other );

        void setSelectedIndex( int i );

        void setSelected( const QString& selected );

        void add( const QString& str, int value );

        void addLast( const QString& str );

        int getSelectedIndex() const;

        int getSelectedValue() const throw (PlvRuntimeException);

        QString getItemName( int index ) const throw (PlvRuntimeException);

        QString getSelectedItemName() const throw (PlvRuntimeException);

        int getItemValue( int index ) const throw (PlvRuntimeException);

        QStringList getItemNames() const;

        QString toString() const;

    protected:
        int m_selectedIndex;
        QMap<int, EnumPair> m_items;
    };

    class EnumPair
    {
    public:
        EnumPair( const QString& s="", int v=-1 ) :
                m_name(s), m_value(v) {}

        inline QString name() const { return m_name; }
        inline int value() const { return m_value; }

    protected:
        QString m_name;
        int m_value;
    };
}
Q_DECLARE_METATYPE( plv::Enum )

#define PLV_ENUM_ADD( plvEnum, type ) plvEnum.add( #type, type )

#endif
