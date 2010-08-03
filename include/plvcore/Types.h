#ifndef TYPES_H
#define TYPES_H

#include <QMetaType>
#include "RefPtr.h"
#include "assert.h"

namespace plv 
{
    class PlvEnum
    {
    public:
        PlvEnum() : m_selected( 0 ) {}
        ~PlvEnum() {}

        PlvEnum( const PlvEnum& other ) :
            m_selected( other.m_selected ),
            m_items( other.m_items )
        {
        }

        void setSelected( int i )
        {
            assert( m_items.contains( i ) );
            m_selected = i;
        }

        void setSelected( const QString& selected )
        {
            int i = 0;
            foreach( const QString& str , m_items.values() )
            {
                if( str.compare( selected ) == 0 )
                {
                    m_selected = i;
                    break;
                }
                ++i;
            }
        }

        int getSelected() const
        {
            return m_selected;
        }

        QString getString( int i )
        {
            if( m_items.contains(i) )
                return m_items[i];
            else
                return "INVALID";
        }

    protected:
        int m_selected;
        QMap<int, QString> m_items;
    };


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

}
/** declare the RefPtr template instantiated with the Data class as a Qt Metatype 
  *  so we can pass RefPtr<Data> along with signals and slots across thread boundaries
  */
Q_DECLARE_METATYPE( plv::RefPtr<plv::Data> )
Q_DECLARE_METATYPE( plv::PlvEnum )

#endif // TYPES_H
