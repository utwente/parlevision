#ifndef TYPES_H
#define TYPES_H

#include <QMap>
#include <QStringList>
#include <QMetaType>

#include "RefPtr.h"
#include "assert.h"

namespace plv 
{
    /** Base class for data resources.
      *
      */
    class Data : public RefCounted
    {
    protected:
        /** mutable flag used for determining if data resource can be
          * written to
          */
        bool m_mutable;

    public:
        Data() : m_mutable( true ) {}

        /** Copy constructor needs to be implemented by super classes
          * to allow the copying of a data resources when the Pin
          * connection type is set to copy which can be faster with
          * simple types.
          */
        Data( const Data& other );

        /** makes this data unit mutable again. Internal framework method.
          * Should normally not be called by client code
          */
        inline void makeMutable()
        {
            QMutexLocker( &this->m_refMutex );
            m_mutable = true;
        }

        /** makes this data element immutable. This is called by the framework when this element
          * is read only shared in multiple threads
          */
        inline void makeImmutable()
        {
            QMutexLocker( &this->m_refMutex );
            m_mutable = false;
        }

        inline bool isMutable() const
        {
            QMutexLocker( &this->m_refMutex );
            return m_mutable;
        }

    protected:
        /** protected destructor, data resources are not allowed to be
          * deleted by individual processor since they can be in use
          * by other processors in other threads. Deletion in one processor
          * could cause a crash. Reference counting is used to let data
          * resources delete themselves.
          */
        virtual ~Data() {}
    };

    /** private class used to store enum information in Enum class */
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

    /** Class for configurable enum properties with introspection support. Useful in GUI code */
    class Enum
    {
    public:
        Enum( int selected = 0 );

        ~Enum();

        Enum( const Enum& other );

        void setSelectedIndex( int i );

        void setSelected( const QString& selected );

        void add( const QString& str );

        void add( const QString& str, int value );

        int getSelectedIndex() const;

        int getSelectedValue() const;

        QString getItemName( int i ) const;

        int getItemValue( int i ) const;

        QStringList getItemNames() const;

        QString toString() const;

    protected:
        int m_selectedIndex;
        QMap<int, EnumPair> m_items;
    };
}
/** Declare as Qt Metatype so we can pass RefPtr<Data> along with signals and slots */
Q_DECLARE_METATYPE( plv::RefPtr<plv::Data> )
Q_DECLARE_METATYPE( plv::Enum )

#endif // TYPES_H
