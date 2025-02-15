//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/KDTree.h - Released 2024-12-28T16:53:48Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2024 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (https://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#ifndef __PCL_KDTree_h
#define __PCL_KDTree_h

/// \file pcl/KDTree.h

#include <pcl/Defs.h>

#include <pcl/Array.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class KDTree
 * \brief Bucket PR K-d tree for point data in arbitrary dimensions.
 *
 * An n-dimensional K-d tree is a specialized binary tree for partitioning of
 * a set of points in an n-dimensional space. K-d trees have important
 * applications in computational geometry problems requiring efficient
 * rectangular range searching.
 *
 * This class implements a <em>bucket point region K-d tree</em> structure
 * (see Reference 2).
 *
 * The template type argument T represents the type of a \e point object stored
 * in a %KDTree structure. The type T must have the following properties:
 *
 * \li The standard default and copy constructors are required:\n
 * \n
 * T::T() \n
 * T::T( const T& )
 *
 * \li The \c T::component subtype must be defined. It represents a component
 * of an object of type T. For example, if T is a vector type, T::component
 * must be the type of a vector component.
 *
 * \li The array subscript operator must be defined as follows:\n
 * \n
 * component T::operator []( int i ) const \n
 * \n
 * This operator must return the value of the i-th component of an object being
 * stored in the K-d tree, such that 0 <= i < N, where N > 0 is the dimension
 * of the point space.
 *
 * \note We use this implementation of K-d trees in some essential PixInsight
 * tools with success (e.g., StarAlignment), and hopefully it will be also
 * useful for you, but we don't claim it to be complete. This is a practical
 * and relatively simple implementation, where only the construction,
 * destruction and range search operations are available. In particular, this
 * implementation does not include point addition, deletion and iteration
 * operations. Future versions of PCL will include more complete
 * implementations of this fundamental data structure.
 *
 * \b References
 *
 * \li 1. Mark de Berg et al., <em>Computational Geometry: Algorithms and
 * Applications Third Edition,</em> Springer, 2010, Section 5.2.
 *
 * \li 2. Hanan Samet, <em>Foundations of Multidimensional and Metric Data
 * Structures,</em> Morgan Kaufmann, 2006, Section 1.5.
 *
 * \sa QuadTree
 */
template <class T>
class PCL_CLASS KDTree
{
public:

   /*!
    * Represents an N-dimensional point stored in this K-d tree.
    */
   using point = T;

   /*!
    * Represents a point component.
    */
   using component = typename point::component;

   /*!
    * A vector of point components. Used internally for tree build and range
    * search operations.
    */
   using component_vector = GenericVector<component>;

   /*!
    * A list of points. Used for tree build and search operations.
    */
   using point_list = Array<point>;

   /*!
    * Constructs an empty K-d tree.
    */
   KDTree() = default;

   /*!
    * Constructs a K-d tree and builds it for the specified list of \a points.
    *
    * \param points           A list of points that will be stored in this
    *                         K-d tree.
    *
    * \param bucketCapacity   The maximum number of points in a leaf tree node.
    *                         Must be >= 1. The default value is 16.
    *
    * The dimension of the point space is taken as the length of the first
    * point in the list (by calling points[0].Length()), and must be > 0. All
    * points in the \a points list must be able to provide at least
    * \a dimension components through a zero-based array subscript operator.
    *
    * If the specified list of \a points is empty, this constructor yields an
    * empty K-d tree. If the dimension of the point space is less than one, an
    * Error exception is thrown.
    */
   KDTree( const point_list& points, int bucketCapacity = 16 )
   {
      Build( points, bucketCapacity );
   }

   /*!
    * Constructs a K-d tree of the specified \a dimension and builds it for a
    * list of \a points.
    *
    * \param points           A list of points that will be stored in this
    *                         K-d tree.
    *
    * \param dimension        The dimension of the point space. Must be > 0.
    *
    * \param bucketCapacity   The maximum number of points in a leaf tree node.
    *                         Must be >= 1.
    *
    * All points in the \a points list must be able to provide at least
    * \a dimension components through a zero-based array subscript operator.
    *
    * If the specified list of \a points is empty, this constructor yields an
    * empty K-d tree. If the dimension of the point space is less than one, an
    * Error exception is thrown.
    */
   KDTree( const point_list& points, int dimension, int bucketCapacity )
   {
      Build( points, dimension, bucketCapacity );
   }

   /*!
    * Copy constructor. Copy construction is disabled because this class uses
    * internal data structures that cannot be copy-constructed. However,
    * %KDTree implements move construction and move assignment.
    */
   KDTree( const KDTree& ) = delete;

   /*!
    * Copy assignment operator. Copy assignment is disabled because this class
    * uses internal data structures that cannot be copy-assigned. However,
    * %KDTree implements move assignment and move construction.
    */
   KDTree& operator =( const KDTree& ) = delete;

   /*!
    * Move constructor.
    */
   KDTree( KDTree&& x )
      : m_root( x.m_root )
      , m_dimension( x.m_dimension )
      , m_bucketCapacity( x.m_bucketCapacity )
      , m_length( x.m_length )
   {
      x.m_root = nullptr;
      x.m_length = 0;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   KDTree& operator =( KDTree&& x )
   {
      if ( &x != this )
      {
         DestroyTree( m_root );
         m_root = x.m_root;
         m_dimension = x.m_dimension;
         m_bucketCapacity = x.m_bucketCapacity;
         m_length = x.m_length;
         x.m_root = nullptr;
         x.m_length = 0;
      }
      return *this;
   }

   /*!
    * Destroys a K-d tree. All the stored point objects are deleted.
    */
   ~KDTree()
   {
      Clear();
   }

   /*!
    * Removes all the stored point objects, yielding an empty K-d tree.
    */
   void Clear()
   {
      DestroyTree( m_root );
      m_root = nullptr;
      m_length = 0;
   }

   /*!
    * Builds a new K-d tree for the specified list of \a points.
    *
    * \param points           A list of points that will be stored in this
    *                         K-d tree.
    *
    * \param bucketCapacity   The maximum number of points in a leaf tree node.
    *                         Must be >= 1. The default value is 16.
    *
    * The dimension of the point space is taken as the length of the first
    * point in the list (by calling points[0].Length()), and must be > 0. All
    * points in the \a points list must be able to provide at least
    * \a dimension components through a zero-based array subscript operator.
    *
    * If the tree stores point objects before calling this function, they are
    * destroyed and removed before building a new tree.
    *
    * If the specified list of \a points is empty, this member function yields
    * an empty K-d tree. If the dimension of the point space is less than one,
    * an Error exception is thrown.
    */
   void Build( const point_list& points, int bucketCapacity = 16 )
   {
      Clear();
      m_bucketCapacity = Max( 1, bucketCapacity );
      if ( !points.IsEmpty() )
      {
         m_dimension = points[0].Length();
         if ( m_dimension < 1 )
            throw Error( "KDTree::Build(): Invalid point space dimension." );
         m_root = BuildTree( points, 0 );
      }
   }

   /*!
    * Builds a new K-d tree of the specified \a dimension for a list of
    * \a points.
    *
    * \param points           A list of points that will be stored in this
    *                         K-d tree.
    *
    * \param dimension        The dimension of the point space. Must be > 0.
    *
    * \param bucketCapacity   The maximum number of points in a leaf tree node.
    *                         Must be >= 1.
    *
    * All points in the \a points list must be able to provide at least
    * \a dimension components through a zero-based array subscript operator.
    *
    * If the tree stores point objects before calling this function, they are
    * destroyed and removed before building a new tree.
    *
    * If the specified list of \a points is empty, this member function yields
    * an empty K-d tree. If the dimension of the point space is less than one,
    * an Error exception is thrown.
    */
   void Build( const point_list& points, int dimension, int bucketCapacity )
   {
      Clear();
      m_bucketCapacity = Max( 1, bucketCapacity );
      if ( (m_dimension = dimension) < 1 )
         throw Error( "KDTree::Build(): Invalid point space dimension." );
      m_root = BuildTree( points, 0 );
   }

   /*!
    * Performs a range search in this K-d tree.
    *
    * \param pt      Reference to the point being searched for. The coordinates
    *                of this point define the center of the hyper-rectangular
    *                search range in the N-dimensional point space.
    *
    * \param epsilon Search tolerance, or half-side of the search
    *                hyperrectangle.
    *
    * Returns a (possibly empty) list with all the points found in the tree
    * within the search range. In two dimensions, the search range would be the
    * rectangle defined by the points:
    *
    * (pt[0] - epsilon, pt[1] - epsilon) and \n
    * (pt[0] + epsilon, pt[1] + epsilon)
    *
    * with an obvious extension to higher dimensions. If \a epsilon is zero,
    * the search can only return the set of stored points that are identical to
    * the specified search point.
    */
   point_list Search( const point& pt, component epsilon ) const
   {
      component_vector p0( m_dimension );
      component_vector p1( m_dimension );
      for ( int i = 0; i < m_dimension; ++i )
      {
         p0[i] = pt[i] - epsilon;
         p1[i] = pt[i] + epsilon;
      }
      point_list found;
      SearchTree( found, p0, p1, m_root, 0 );
      return found;
   }

   /*!
    * Performs a range search in this K-d tree.
    *
    * \param pt         Reference to the point being searched for. The
    *                   coordinates of this point define the center of the
    *                   hyper-rectangular search range in the N-dimensional
    *                   point space.
    *
    * \param epsilon    Search tolerance, or half-side of the search
    *                   hyperrectangle.
    *
    * \param callback   Callback functional.
    *
    * \param data       Callback data.
    *
    * The callback function prototype should be:
    *
    * \code void callback( const point& pt, void* data ) \endcode
    *
    * The callback function will be called once for each point found in the
    * tree within the specified search range. In two dimensions, the search
    * range would be the rectangle defined by the points:
    *
    * (pt[0] - epsilon, pt[1] - epsilon) and \n
    * (pt[0] + epsilon, pt[1] + epsilon)
    *
    * with an obvious extension to higher dimensions. If \a epsilon is zero,
    * the search can only return the set of stored points that are identical to
    * the specified search point.
    */
   template <class F>
   void Search( const point& pt, component epsilon, F callback, void* data ) const
   {
      component_vector p0( m_dimension );
      component_vector p1( m_dimension );
      for ( int i = 0; i < m_dimension; ++i )
      {
         p0[i] = pt[i] - epsilon;
         p1[i] = pt[i] + epsilon;
      }
      SearchTree( p0, p1, callback, data, m_root, 0 );
   }

   /*!
    * Returns the dimension of this K-d tree. This is the number of components
    * in a point stored in the tree.
    */
   int Dimension() const
   {
      return m_dimension;
   }

   /*!
    * Returns the total number of points stored in this K-d tree.
    */
   size_type Length() const
   {
      return m_length;
   }

   /*!
    * Returns true iff this K-d tree is empty.
    */
   bool IsEmpty()
   {
      return m_root == nullptr;
   }

   /*!
    * Exchanges two %KDTree objects \a x1 and \a x2.
    */
   friend void Swap( KDTree& x1, KDTree& x2 )
   {
      pcl::Swap( x1.m_root,           x2.m_root );
      pcl::Swap( x1.m_dimension,      x2.m_dimension );
      pcl::Swap( x1.m_bucketCapacity, x2.m_bucketCapacity );
      pcl::Swap( x1.m_length,         x2.m_length );
   }

private:

   struct Node
   {
      double split = 0;       // position of this node's splitting hyperplane
      Node*  left = nullptr;  // child points at coordinates <= split
      Node*  right = nullptr; // child points at coordinates > split

      Node( double s = 0 )
         : split( s )
      {
      }

      bool IsLeaf() const
      {
         return left == nullptr && right == nullptr;
      }
   };

   struct LeafNode : public Node
   {
      point_list points;

      LeafNode( const point_list& p )
         : points( p )
      {
      }
   };

   Node*     m_root = nullptr;
   int       m_dimension = 0;
   int       m_bucketCapacity = 0;
   size_type m_length = 0;

   LeafNode* NewLeafNode( const point_list& points )
   {
      m_length += points.Length();
      return new LeafNode( points );
   }

   Node* BuildTree( const point_list& points, int depth )
   {
      if ( points.IsEmpty() )
         return nullptr;

      if ( points.Length() <= size_type( m_bucketCapacity ) )
         return NewLeafNode( points );

      int index = depth % m_dimension;

      Node* node = new Node( SplitValue( points, index ) );

      point_list left, right;
      for ( const point& p : points )
         if ( p[index] <= node->split )
            left.Add( p );
         else
            right.Add( p );

      // If we are about to build a degenerate subtree, abort this branch of
      // recursion right now.
      if ( left.IsEmpty() || right.IsEmpty() )
      {
         delete node;
         return NewLeafNode( points );
      }

      node->left  = BuildTree( left, depth+1 );
      node->right = BuildTree( right, depth+1 );

      // Further degeneracies cannot happen in theory, but let's prevent them
      // for extra safety.
      if ( node->IsLeaf() )
      {
         delete node;
         return NewLeafNode( points );
      }

      return node;
   }

   void SearchTree( point_list& found, const component_vector& p0, const component_vector& p1, const Node* node, int depth ) const
   {
      if ( node != nullptr )
         if ( node->IsLeaf() )
         {
            const LeafNode* leaf = static_cast<const LeafNode*>( node );
            for ( const point& p : leaf->points )
               for ( int j = 0; ; )
               {
                  component x = p[j];
                  if ( x < p0[j] || p1[j] < x )
                     break;
                  if ( ++j == m_dimension )
                  {
                     found.Add( p );
                     break;
                  }
               }
         }
         else
         {
            int index = depth % m_dimension;
            if ( p0[index] <= node->split )
               SearchTree( found, p0, p1, node->left, depth+1 );
            if ( p1[index] > node->split )
               SearchTree( found, p0, p1, node->right, depth+1 );
         }
   }

   template <class F>
   void SearchTree( const component_vector& p0, const component_vector& p1, F callback, void* data, const Node* node, int depth ) const
   {
      if ( node != nullptr )
         if ( node->IsLeaf() )
         {
            const LeafNode* leaf = static_cast<const LeafNode*>( node );
            for ( const point& p : leaf->points )
               for ( int j = 0; ; )
               {
                  component x = p[j];
                  if ( x < p0[j] || p1[j] < x )
                     break;
                  if ( ++j == m_dimension )
                  {
                     callback( p, data );
                     break;
                  }
               }
         }
         else
         {
            int index = depth % m_dimension;
            if ( p0[index] <= node->split )
               SearchTree( p0, p1, callback, data, node->left, depth+1 );
            if ( p1[index] > node->split )
               SearchTree( p0, p1, callback, data, node->right, depth+1 );
         }
   }

   void DestroyTree( Node* node )
   {
      if ( node != nullptr )
         if ( node->IsLeaf() )
            delete static_cast<LeafNode*>( node );
         else
         {
            DestroyTree( node->left );
            DestroyTree( node->right );
            delete node;
         }
   }

   double SplitValue( const point_list& points, int index )
   {
      component_vector v( points.Length() );
      for ( int i = 0; i < v.Length(); ++i )
         v[i] = points[i][index];
      return v.Median();
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_KDTree_h

// ----------------------------------------------------------------------------
// EOF pcl/KDTree.h - Released 2024-12-28T16:53:48Z
