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

        /** adds a flied with id 'str' and value 'value' to the end of the enum */
        void add( const QString& str, int value );

        /** calls add with value of the index of this string in the enum
            so doing add( "Value 0" ) is equivalent to add( "Value 0", 0 )
            on an empty enum */
        void add( const QString& str );

        int getSelectedIndex() const;

        int getSelectedValue() const throw (RuntimeError);

        QString getItemName( int index ) const throw (RuntimeError);

        QString getSelectedItemName() const throw (RuntimeError);

        int getItemValue( int index ) const throw (RuntimeError);

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
