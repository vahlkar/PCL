#ifndef EXCLUSIVEACCESS_H
#define EXCLUSIVEACCESS_H

#include <pcl/AutoLock.h>

namespace pcl
{

// ----------------------------------------------------------------------------

template <class T>
class ExclusiveAccess
{
public:

   typedef T   item_type;

   ExclusiveAccess( Mutex& mutex, item_type& item ) :
      m_lock( mutex ),
      m_item( item )
   {
   }

   operator item_type&()
   {
      return m_item;
   }

   operator const item_type&() const
   {
      return m_item;
   }

private:

   AutoLock   m_lock;
   item_type& m_item;
};

template <class T>
class ExclusiveConstAccess
{
public:

   typedef T   item_type;

   ExclusiveConstAccess( Mutex& mutex, const item_type& item ) :
      m_lock( mutex ),
      m_item( item )
   {
   }

   operator const item_type&() const
   {
      return m_item;
   }

private:

         AutoLock   m_lock;
   const item_type& m_item;
};

}
#endif // EXCLUSIVEACCESS_H
