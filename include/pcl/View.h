//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/View.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_View_h
#define __PCL_View_h

/// \file pcl/View.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Array.h>
#include <pcl/Histogram.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/ImageVariant.h>
#include <pcl/Property.h>
#include <pcl/UIObject.h>
#include <pcl/Variant.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS ImageWindow;

// ----------------------------------------------------------------------------

/*!
 * \defgroup view_properties Module-Defined View Properties
 */

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ViewPropertyAttribute
 * \brief     Attributes of view properties.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr>
 *    <td>ViewPropertyAttribute::WriteProtected</td>
 *    <td>Only the module that has created the property can modify it (e.g.,
 *        change its value, or delete it).</td>
 * </tr>
 * <tr>
 *    <td>ViewPropertyAttribute::ReadProtected</td>
 *    <td>Only the module that has created the property can read its value.
 *        Implies write protection, even if the WriteProtected attribute has
 *        not been set explicitly.</td>
 * </tr>
 * <tr>
 *    <td>ViewPropertyAttribute::Volatile</td>
 *    <td>Volatile properties are not stored in processing histories and get
 *        lost across undo/redo operations.</td>
 * </tr>
 * <tr>
 *    <td>ViewPropertyAttribute::Permanent</td>
 *    <td>Permanent properties are not stored in processing histories, but
 *        unlike volatile properties, they are preserved across undo/redo
 *        operations. A permanent property is not deleted automatically when
 *        the current process terminates. Permanent properties are mostly
 *        useful to store metadata that does not depend on pixel values, such
 *        as astrometric solutions, or image acquisition conditions and similar
 *        ancillary data.</td></tr>
 * <tr>
 *    <td>ViewPropertyAttribute::NotSerializable</td>
 *    <td>The property will not be stored in projects.</td>
 * </tr>
 * <tr>
 *    <td>ViewPropertyAttribute::Storable</td>
 *    <td>The property can be stored in XISF files.</td>
 * </tr>
 * <tr>
 *    <td>ViewPropertyAttribute::Reserved</td>
 *    <td>The property has been reserved by the PixInsight core application. It
 *        can only be generated by calling the View::ComputeProperty() member
 *        function. This attribute cannot be set explicitly by a modue.</td>
 * </tr>
 * <tr>
 *    <td>ViewPropertyAttribute::Protected</td>
 *    <td>The property will not be deleted after completing execution of the
 *        current process, even if the Permanent attribute is not set. This
 *        special flag will be removed automatically after process execution.
 *        A protected property represents ancillary data that depends on pixel
 *        values and has been generated by the process that has just finished
 *        its execution.</td>
 * </tr>
 * <tr>
 *    <td>ViewPropertyAttribute::NoChange</td>
 *    <td>This is a special flag used to preserve the existing property
 *        attributes. Normally it is intended for internal PCL use.</td>
 * </tr>
 * </table>
 *
 * \ingroup view_properties
 */
namespace ViewPropertyAttribute
{
   enum mask_type
   {
      WriteProtected  = 0x00000001,
      ReadProtected   = 0x00000002,
      Volatile        = 0x00000010,
      Permanent       = 0x00000020,
      NotSerializable = 0x00000040,
      Storable        = 0x00000080,
      Reserved        = 0x10000000,
      Protected       = 0x20000000,
      NoChange        = 0x80000000
   };
}

/*!
 * A combination of ViewPropertyAttribute flags.
 * \ingroup view_properties
 */
using ViewPropertyAttributes = Flags<ViewPropertyAttribute::mask_type>;

// ----------------------------------------------------------------------------

/*!
 * \class View
 * \brief High-level interface to a PixInsight view object.
 *
 * Instances of %View are managed aliases of actual view objects in the
 * PixInsight core application. Views are the main image holders in the
 * PixInsight platform.
 *
 * Views can be <em>main views</em> or \e previews. A main view holds the
 * entire image of an image window. A preview is a volatile subimage defined on
 * an image window. In all respects, both types of views can be manipulated
 * transparently with the abstract interface provided by the %View class.
 * The View::IsPreview() and View::IsMainView() members inform you about the
 * particular type of a given %View object.
 *
 * Each view has a unique identifier, which you can get and set through the
 * View::Id(), View::FullId(), and View::Rename() member functions.
 *
 * View::Image() gives you full access to the image in a view as an
 * ImageVariant object.
 *
 * Each view in PixInsight holds a set of histograms and statistical data as
 * shared resources that can be accessed by all installed processes and
 * interfaces. The View::CalculateHistograms() and View::CalculateStatistics()
 * families of member functions give full access to these shared resources.
 *
 * In a similar way, each view maintains a set of histogram transformations
 * used as <em>screen transfer functions</em> (STF). A STF is applied to modify
 * the screen representation of an image, without altering actual image data.
 * The View::GetScreenTransferFunctions() / View::SetScreenTransferFunctions()
 * family of functions allow you to acquire and modify the STF of any view.
 *
 * The View::Lock() / View::Unlock() family of functions allow you to write
 * thread-safe processing routines in current and future PixInsight
 * multithreaded environments.
 *
 * Finally, the static functions View::ViewById(), View::AllViews() and
 * View::AllPreviews() provide for global searching and listing of existing
 * view objects.
 *
 * \sa ImageWindow
 */
class PCL_CLASS View : public UIObject
{
public:

   /*!
    * A container of HistogramTransformation instances used to represent the
    * managed Screen Transfer Functions (STF) of a view object in the core
    * PixInsight application.
    */
   using stf_list = Array<HistogramTransformation>;

   /*!
    * Constructs a null view. A null view does not correspond to an existing
    * view in the PixInsight core application.
    */
   View() = default;

   /*!
    * Constructs a %View instance as an alias of an existing %View object.
    *
    * \note It cannot be overemphasized that this constructor <em>does not
    * create a new view</em>. It only creates an \e alias object for an
    * existing view <em>in the calling module</em>. In all respects, the alias
    * and aliased objects are completely interchangeable; they behave exactly
    * in the same way because both refer to the same server-side object.
    */
   View( const View& v )
      : UIObject( v )
   {
   }

   /*!
    * Destroys a %View object.
    *
    * Note that this destructor does not destroy the actual view object, which
    * is part of the PixInsight core application. Only the managed alias object
    * living in the user-defined module is destroyed.
    */
   ~View() override
   {
   }

   /*!
    * Returns a reference to a null %View instance. A null %View does not
    * correspond to an existing view in the PixInsight core application.
    */
   static View& Null();

   /*!
    * Returns true iff this is a main view. A main view holds the entire image
    * in an image window.
    */
   bool IsMainView() const;

   /*!
    * Returns true iff this view corresponds to a preview object. A preview is
    * a volatile subimage defined in an image window.
    */
   bool IsPreview() const;

   /*!
    * Returns true iff this view is a preview and it does not cover its parent
    * main view completely.
    */
   bool IsPartialPreview() const;

   /*!
    * Returns true iff this view is a preview and it covers its parent main
    * view completely.
    */
   bool IsCompletePreview() const;

   /*!
    * Returns true iff this view is a preview and is in volatile state. A
    * volatile preview contains a temporary image that can be undone/redone
    * with the <em>Preview > Undo</em> and <em>Preview > Redo</em> core
    * application commands.
    */
   bool IsVolatilePreview() const;

   /*!
    * Returns true iff this view is a preview and has one or more stored states.
    * When a preview has been stored, it behaves as an independent image with
    * its own processing history. A stored preview does not rely on its mother
    * image to return to a 'base' state.
    */
   bool IsStoredPreview() const;

   /*!
    * Returns a managed alias for the image window this view pertains to.
    */
   ImageWindow Window() const;

   /*!
    * Returns the identifier of this view.
    *
    * %View identifiers are unique within their naming context. Identifiers of
    * main views are unique in the global context, since each main view holds
    * an image in the core application. The identifier of a preview is unique
    * in the context of its parent image window.
    */
   IsoString Id() const;

   /*!
    * Returns the full identifier of this view.
    *
    * If this is a main view, this function returns its identifier, which is
    * the same string returned by the Id() member function.
    *
    * If this view corresponds to a preview, this function returns a unique
    * identifier of the form:
    *
    * <tt>\<image_id\>-\>\<id\></tt>
    *
    * where \<image_id\> is the identifier of the preview's parent image, and
    * \<id\> is the identifier of the preview. The "->" sequence is the
    * <em>scope resolution</em> operator, used to specify pertenence of a
    * preview to its parent image in the PixInsight environment.
    */
   IsoString FullId() const;

   /*!
    * Changes the identifier of this view to \a newId.
    *
    * If \a newId is not unique in the naming context of this view, a unique
    * identifier is obtained automatically by appending a suffix string to the
    * specified \a newId string.
    *
    * After calling this function, a ImageRenamed() notification message will
    * be sent to all process interfaces.
    */
   void Rename( const IsoString& newId );

   void Rename( const IsoString::ustring_base& newId )
   {
      Rename( IsoString( newId ) );
   }

   /*!
    * Returns true iff this view is not locked for reading.
    */
   bool CanRead() const;

   /*!
    * Returns true iff this view is not locked for writing.
    */
   bool CanWrite() const;

   /*!
    * Locks this view for read and write operations.
    *
    * Processes usually call this function to make sure that a target view's
    * image cannot be modified by another thread while they are processing it.
    * This is necessary because PixInsight is a multithreaded environment. If a
    * process modifies an image without locking it, other processing threads
    * could try to read or write the same pixels concurrently, with
    * unpredictable results.
    *
    * If the view is not currently locked by other processing thread, this
    * function locks it and returns immediately. If the view is already
    * locked, this function waits until it becomes unlocked by other threads.
    *
    * If the \a notify argument is false, this function will not send
    * ViewLocked() notifications to process interfaces.
    *
    * \warning Always make sure that you unlock a view that you have previously
    * locked. If your processing routines leave views locked incorrectly,
    * they'll cause serious problems. A locked view is inaccessible to other
    * processes, and even the user may be unable to recover locked image data.
    */
   void Lock( bool notify = true ) const;

   /*!
    * Unlocks this view for read and write operations.
    *
    * Please read the \e important information given for the Lock() function
    * member.
    *
    * You shouldn't call this function if your routines have not called the
    * Lock() function before. In other words, call Unlock() just once for each
    * previous call lo Lock().
    *
    * \warning Be aware that \e just after this function returns, your
    * processing routine \e must \e not try to modify this view or its
    * associated image, since other thread might have locked it. If you need
    * to gain exclusive read/write rights over this view, call Lock() again.
    */
   void Unlock( bool notify = true ) const;

   /*!
    * Locks this view for write operations only.
    *
    * Call this function if you need to make sure that a view's image will not
    * be modified by other threads, but your processing routines will not
    * modify it.
    *
    * For example, if your routines need reading this view's image as source
    * data for your processing tasks, but not writing to it, you should call
    * this function instead of Lock(). In this way you allow other processes
    * (including the core application's GUI) to access this view and its image
    * for read-only operations. This improves efficiency and useability of the
    * whole PixInsight environment.
    */
   void LockForWrite( bool notify = true ) const;

   /*
    * Unlocks this view for write operations only.
    * ### Undocumented (i.e., harmful) function.
    */
   void UnlockForWrite( bool notify = true ) const;

   /*!
    * Temporarily unlocks a view for reading.
    *
    * If your code has successfully called Lock() for this view, then its
    * screen rendition cannot be updated, because the core application will
    * never try to read pixel data from a view that is read-locked. This can be
    * a problem if, for example, you need to refresh this view's screen
    * rendition to provide feedback to the user at a given stage of your
    * processing.
    *
    * The solution for such situations is calling this function to temporarily
    * permit read-only accesses to this view and its image. When you no longer
    * want to allow read-only operations for this view, call RelockForRead()
    * and continue your processing work.
    *
    * Your code may call Unlock() safely after this function; you should not
    * call RelockForRead() if your processing task has finished, or if this
    * view is no longer needed for it.
    *
    * \warning Do not use this function if your code has not called Lock()
    * previously.
    */
   void UnlockForRead( bool notify = true ) const;

   /*!
    * Relocks a view for reading (after UnlockForRead()).
    *
    * Call this function after UnlockForRead(), if necessary. Please read the
    * information given for that function.
    */
   void RelockForRead( bool notify = true ) const;

   /*!
    * Returns true iff this view has been selected as a target of an active
    * dynamic interface.
    */
   bool IsDynamicTarget() const;

   /*!
    * Adds this view to the list of targets of the current active dynamic
    * interface.
    *
    * Dynamic interfaces call this function for views that get involved in
    * active dynamic procedures. For example, the DynamicCrop interface calls
    * this function when you select a view by defining an initial cropping
    * rectangle with the mouse.
    *
    * Any images selected as dynamic targets cannot be closed or modified in
    * any way while an active dynamic interface depends on them.
    *
    * If you implement a dynamic interface, always call this function for a
    * view if your dynamic interface depends on it. Failing to do so will lead
    * to unstable and incoherent behavior of the core application's GUI.
    *
    * \warning Once your dynamic interface ceases depending on this view, call
    * the RemoveFromDynamicTargets() function for it.
    */
   void AddToDynamicTargets();

   /*!
    * Removes this view from the list of targets of the current active
    * dynamic interface.
    *
    * Call this function when your dynamic interface no longer depends on this
    * view. Please read the important information given for
    * AddToDynamicTargets().
    */
   void RemoveFromDynamicTargets();

   /*!
    * Returns an ImageVariant instance that transports the image in this view.
    *
    * The returned ImageVariant object transports a <em>shared image</em>. A
    * shared image is a managed alias for an actual image living in the core
    * PixInsight application. This is because a view is also a managed object.
    *
    * With this function you gain access to the image in a view, which is
    * crucial to perform any kind of processing in the PixInsight/PCL
    * framework.
    *
    * Before calling this function, however, you must make sure that your
    * processing thread has the appropriate access rights to the view, since
    * PixInsight is a multithreaded environment. This is done by calling the
    * Lock() member function of %View.
    */
   ImageVariant Image() const;

   /*!
    * Returns an ImageVariant instance that transports the image in this view.
    *
    * \deprecated Use View::Image() in newly produced code.
    */
   ImageVariant GetImage() const
   {
      return this->Image();
   }

   /*!
    * Returns true iff this view holds a color image, false if it is a
    * grayscale image.
    */
   bool IsColor() const;

   /*!
    * Copies the width and height in pixels of the image in this view to the
    * specified variables.
    */
   void GetSize( int& width, int& height ) const;

   /*!
    * Returns the width in pixels of the image in this view.
    */
   int Width() const
   {
      int w, dum; GetSize( w, dum ); return w;
   }

   /*!
    * Returns the height in pixels of the image in this view.
    */
   int Height() const
   {
      int dum, h; GetSize( dum, h ); return h;
   }

   /*!
    * Returns the bounding rectangle of the image in this view. The upper left
    * corner of the returned rectangle (x0, y0) is always (0,0). The lower
    * right corner coordinates (x1, y1) correspond to the width and height of
    * the image.
    */
   Rect Bounds() const
   {
      int w, h; GetSize( w, h ); return Rect( w, h );
   }

   /*!
    * Retrieves the set of screen transfer functions (STF) for this view in the
    * specified container.
    *
    * The STF container is a dynamic array. Each array element is a
    * HistogramTransformation object corresponding to the STF for an image
    * channel or color component:
    *
    * %Array element #0 = Red/gray channels \n
    * %Array element #1 = Green channel \n
    * %Array element #2 = Blue channel \n
    * %Array element #3 = Lightness/brightness/luminance components
    */
   void GetScreenTransferFunctions( stf_list& ) const;

   /*!
    * Sets the screen transfer functions (STF) for this view.
    *
    * The specified container is a dynamic array. Each array element is a
    * HistogramTransformation object corresponding to the STF for an image
    * channel or color component:
    *
    * %Array element #0 = Red/gray channels \n
    * %Array element #1 = Green channel \n
    * %Array element #2 = Blue channel \n
    * %Array element #3 = Lightness/brightness/luminance components
    */
   void SetScreenTransferFunctions( const stf_list&, bool notify = true );

   /*!
    * Destroys the screen transfer functions (STF) for this view.
    */
   void DestroyScreenTransferFunctions( bool notify = true );

   /*!
    * Returns true iff screen transfer functions (STF) are enabled for this
    * view. If STFs are disabled, they are not used for screen renditions.
    */
   bool AreScreenTransferFunctionsEnabled() const;

   /*!
    * Enables or disables screen transfer functions (STF) for this view. If
    * STFs are enabled, each non-identity STF is used to modify screen
    * renditions of the corresponding image channel.
    */
   void EnableScreenTransferFunctions( bool = true, bool notify = true );

   /*!
    * Disables or enables screen transfer functions (STF) for this view. If
    * STFs are enabled, each non-identity STF is used to modify screen
    * renditions of the corresponding image channel.
    */
   void DisableScreenTransferFunctions( bool disable = true, bool notify = true )
   {
      EnableScreenTransferFunctions( !disable, notify );
   }

   /*!
    * returns true iff the specified string \a id is the identifier of a
    * reserved view property.
    *
    * Reserved view properties are computed and maintained automatically by the
    * core application and cannot be modified arbitrarily by modules.
    *
    * This member function also returns true if the specified identifier starts
    * with the string "PixInsight:". Although these identifiers are not
    * strictly reserved, the core application defines a number of properties in
    * the PixInsight namespace for its internal use. This namespace should not
    * be used by modules.
    *
    * \sa ComputeProperty()
    * \ingroup view_properties
    */
   static bool IsReservedViewPropertyId( const IsoString& id );

   /*!
    * Returns a description of all data properties associated with this view.
    * For each property, the returned array provides information on the unique
    * identifier of a property and its data type.
    *
    * Returns an empty array if there are no properties in this view.
    *
    * \ingroup view_properties
    */
   PropertyDescriptionArray PropertyDescriptions() const;

   /*!
    * Returns an array with all readable (for the calling module) properties in
    * this view.
    *
    * \ingroup view_properties
    */
   PropertyArray Properties() const;

   /*!
    * Returns an array with all readable (for the calling module) and storable
    * properties in this view.
    *
    * Storable properties have the ViewPropertyAttribute::Storable attribute
    * set and are intended to be persistent when writting view images to files.
    *
    * \sa SetStorableProperties()
    * \ingroup view_properties
    */
   PropertyArray StorableProperties() const;

   /*!
    * Returns an array with all readable (for the calling module) and permanent
    * properties in this view.
    *
    * Permanent properties are not stored in processing histories, but unlike
    * volatile properties, they are preserved across undo/redo operations.
    *
    * \sa SetPermanentProperties()
    * \ingroup view_properties
    */
   PropertyArray PermanentProperties() const;

   /*!
    * Returns an array with all readable (for the calling module), storable,
    * permanent properties in this view.
    *
    * Storable properties have the ViewPropertyAttribute::Storable attribute
    * set and are intended to be persistent when writting view images to files.
    *
    * Permanent properties are not stored in processing histories, but unlike
    * volatile properties, they are preserved across undo/redo operations.
    *
    * \sa SetStorablePermanentProperties()
    * \ingroup view_properties
    */
   PropertyArray StorablePermanentProperties() const;

   /*!
    * Sets the values of a set of properties in this view.
    *
    * \param properties The properties that will be defined.
    *
    * \param notify     Whether to notify the platform on the property changes.
    *                   This is true by default.
    *
    * \param attributes Optional attribute properties. If not specified, the
    *                   current property attributes will be preserved. If not
    *                   specified and the property is newly created, a default
    *                   set of properties will be applied.
    *
    * For each item in the \a properties array, if the requested property is
    * not a reserved property and does not exist in this view, a new one will
    * be created with the specified identifier, value and attributes; see the
    * Property class.
    *
    * If one or more properties exist but the calling module has no write
    * access to them (see ViewPropertyAttribute::WriteProtected), an Error
    * exception will be thrown.
    *
    * Reserved properties are simply ignored by this member function without
    * raising exceptions. This allows for copying properties between views
    * safely with a single-line call such as:
    *
    * \code view2.SetProperties( view1.Properties() ); \endcode
    *
    * \ingroup view_properties
    */
   void SetProperties( const PropertyArray& properties, bool notify = true,
                       ViewPropertyAttributes attributes = ViewPropertyAttribute::NoChange );

   /*!
    * Sets the values of a set of storable properties in this view.
    *
    * Calling this function is equivalent to:
    *
    * \code
    * SetProperties( properties, notify,
    *                ViewPropertyAttribute::Storable
    *              | ViewPropertyAttribute::NoChange )
    * \endcode
    *
    * This function simplifies defining view properties, where the Storable
    * attribute is used very often to ensure that properties will be propagated
    * to newly created and updated disk files.
    *
    * \sa StorableProperties()
    * \ingroup view_properties
    */
   void SetStorableProperties( const PropertyArray& properties, bool notify = true )
   {
      SetProperties( properties, notify,
                     ViewPropertyAttribute::Storable
                   | ViewPropertyAttribute::NoChange );
   }

   /*!
    * Sets the values of a set of storable and permanent properties in this
    * view.
    *
    * Calling this function is equivalent to:
    *
    * \code
    * SetProperties( properties, notify,
    *                ViewPropertyAttribute::Storable
    *              | ViewPropertyAttribute::Permanent
    *              | ViewPropertyAttribute::NoChange )
    * \endcode
    *
    * This function simplifies defining view properties, where the Storable and
    * Permanent attributes are used very often to ensure that properties will
    * be propagated to newly created and updated disk files, as well as
    * preserved in views across process executions and undo/redo operations.
    *
    * \sa StorableProperties(), PermanentProperties()
    * \ingroup view_properties
    */
   void SetStorablePermanentProperties( const PropertyArray& properties, bool notify = true )
   {
      SetProperties( properties, notify,
                     ViewPropertyAttribute::Storable
                   | ViewPropertyAttribute::Permanent
                   | ViewPropertyAttribute::NoChange );
   }

   /*!
    * Returns the value of the specified \a property in this view.
    *
    * If the requested property has not been defined for this view, the
    * returned Variant object will be invalid (that is, Variant::IsValid() will
    * return false).
    *
    * If the property exists but the calling module has no read access to it
    * (see ViewPropertyAttributes::ReadProtected), an Error exception will be
    * thrown.
    *
    * \ingroup view_properties
    */
   Variant PropertyValue( const IsoString& property ) const;

   Variant PropertyValue( const IsoString::ustring_base& property ) const
   {
      return PropertyValue( IsoString( property ) );
   }

   /*!
    * Computes a reserved view property and returns its value.
    *
    * The PixInsight core application reserves a set of view property
    * identifiers for standard use by all modules. These special properties
    * can only be generated by calling this member function; they cannot be
    * created or modified by other means (for example, by calling the
    * SetPropertyValue() and SetPropertyAttributes() functions). This includes
    * a number of statistical properties that are generated and computed on
    * demand in a highly optimized way.
    *
    * The set of reserved view property identifiers includes at least the
    * following list:
    *
    * Mean, Modulus, SumOfSquares, Median, Variance, StdDev, AvgDev, MAD, BWMV,
    * PBMV, Sn, Qn, Minimum, MinimumPos, Maximum, MaximumPos, Histogram16,
    * Histogram20.
    *
    * If the requested property is not recognized as a reserved view property,
    * this member function returns an invalid %Variant object.
    *
    * \sa IsReservedViewPropertyId()
    * \ingroup view_properties
    */
   Variant ComputeProperty( const IsoString& property, bool notify = true );

   Variant ComputeProperty( const IsoString::ustring_base& property, bool notify = true )
   {
      return ComputeProperty( IsoString( property ), notify );
   }

   /*!
    * Returns the value of a reserved view property if it is already available,
    * or computes it otherwise and returns its newly calculated value.
    *
    * This member function is equivalent to the following sequence:
    *
    * \code
    * if ( HasProperty( property ) )
    *    return PropertyValue( property );
    * return ComputeProperty( property, notify );
    * \endcode
    *
    * See ComputeProperty() for information on reserved view properties.
    *
    * \ingroup view_properties
    */
   template <class S>
   Variant ComputeOrFetchProperty( const S& property, bool notify = true )
   {
      IsoString propertyId( property );
      if ( HasProperty( propertyId ) )
         return PropertyValue( propertyId );
      return ComputeProperty( propertyId, notify );
   }

   /*!
    * Sets the value and attributes of a property in this view.
    *
    * \param property   Identifier of the view property.
    *
    * \param value      A valid Variant object transporting the new property
    *                   value.
    *
    * \param notify     Whether to notify the platform on the property change.
    *                   This is true by default.
    *
    * \param attributes Optional attribute properties. If not specified, the
    *                   current property attributes will be preserved. If not
    *                   specified and the property is newly created, a default
    *                   set of properties will be applied.
    *
    * If the requested property does not exist in this view, a new one will be
    * created with the specified identifier, value and attributes.
    *
    * If the property exists but the calling module has no write access to it
    * (see ViewPropertyAttribute::WriteProtected), an Error exception will be
    * thrown.
    *
    * \ingroup view_properties
    */
   void SetPropertyValue( const IsoString& property, const Variant& value, bool notify = true,
                          ViewPropertyAttributes attributes = ViewPropertyAttribute::NoChange );

   void SetPropertyValue( const IsoString::ustring_base& property, const Variant& value, bool notify = true,
                          ViewPropertyAttributes attributes = ViewPropertyAttribute::NoChange )
   {
      SetPropertyValue( IsoString( property ), value, notify, attributes );
   }

   /*!
    * Sets the value of a storable property in this view.
    *
    * This member function is equivalent to:
    *
    * \code
    * SetPropertyValue( property, value, notify,
    *                   ViewPropertyAttribute::Storable
    *                 | ViewPropertyAttribute::NoChange );
    * \endcode
    *
    * This function simplifies defining view properties, where the Storable
    * attribute is used very often to ensure that properties will be propagated
    * to newly created and updated disk files.
    *
    * \ingroup view_properties
    */
   void SetStorablePropertyValue( const IsoString& property, const Variant& value, bool notify = true )
   {
      SetPropertyValue( property, value, notify,
                        ViewPropertyAttribute::Storable
                      | ViewPropertyAttribute::NoChange );
   }

   void SetStorablePropertyValue( const IsoString::ustring_base& property, const Variant& value, bool notify = true )
   {
      SetStorablePropertyValue( IsoString( property ), value, notify );
   }

   /*!
    * Sets the value of a storable property in this view.
    *
    * This member function is equivalent to:
    *
    * \code
    * SetPropertyValue( property, value, notify,
    *                   ViewPropertyAttribute::Storable
    *                 | ViewPropertyAttribute::Permanent
    *                 | ViewPropertyAttribute::NoChange );
    * \endcode
    *
    * This function simplifies defining view properties, where the Storable and
    * Permanent attributes are used very often to ensure that properties will
    * be propagated to newly created and updated disk files, as well as
    * preserved in views across process executions and undo/redo operations.
    *
    * \ingroup view_properties
    */
   void SetStorablePermanentPropertyValue( const IsoString& property, const Variant& value, bool notify = true )
   {
      SetPropertyValue( property, value, notify,
                        ViewPropertyAttribute::Storable
                      | ViewPropertyAttribute::Permanent
                      | ViewPropertyAttribute::NoChange );
   }

   void SetStorablePermanentPropertyValue( const IsoString::ustring_base& property, const Variant& value, bool notify = true )
   {
      SetStorablePropertyValue( IsoString( property ), value, notify );
   }

   /*!
    * Returns the data type of an existing \a property in this view.
    *
    * If the requested \a property has not been defined for this view, this
    * member function returns VariantType::Invalid.
    *
    * If the property exists but the calling module has no read access to it
    * (see ViewPropertyAttributes::ReadProtected), an Error exception will be
    * thrown.
    *
    * For a list of available view property types, see the VariantType
    * namespace.
    *
    * \ingroup view_properties
    */
   Variant::data_type PropertyType( const IsoString& property ) const;

   Variant::data_type PropertyType( const IsoString::ustring_base& property ) const
   {
      return PropertyType( IsoString( property ) );
   }

   /*!
    * Returns the set of attributes currently associated with an existing
    * \a property in this view.
    *
    * If the requested \a property has not been defined for this view, an Error
    * exception is thrown.
    *
    * For a list of available view property attributes, see the
    * ViewPropertyAttribute namespace.
    *
    * \ingroup view_properties
    */
   ViewPropertyAttributes PropertyAttributes( const IsoString& property ) const;

   ViewPropertyAttributes PropertyAttributes( const IsoString::ustring_base& property ) const
   {
      return PropertyAttributes( IsoString( property ) );
   }

   /*!
    * Sets new \a attributes for an existing \a property in this view.
    *
    * If the requested \a property is not currently defined for this view, or
    * if the calling module has no write access to it (see
    * ViewPropertyAttribute::WriteProtected), an Error exception will be
    * thrown.
    *
    * Note that property attributes can be set for a newly created property
    * with the View::SetPropertyValue() member function. For read-only
    * properties, this is safer because the property never exists as a publicly
    * writable object.
    *
    * \ingroup view_properties
    */
   void SetPropertyAttributes( const IsoString& property, ViewPropertyAttributes attributes, bool notify = true );

   void SetPropertyAttributes( const IsoString::ustring_base& property, ViewPropertyAttributes attributes, bool notify = true )
   {
      SetPropertyAttributes( IsoString( property ), attributes, notify );
   }

   /*!
    * Returns true iff the specified \a property exists in this view.
    *
    * \ingroup view_properties
    */
   bool HasProperty( const IsoString& property ) const;

   bool HasProperty( const IsoString::ustring_base& property ) const
   {
      return HasProperty( IsoString( property ) );
   }

   /*!
    * Deletes the specified \a property and its associated value in this view.
    *
    * If the requested \a property is not currently defined for this view, or
    * if the calling module has no write access to it (see
    * ViewPropertyAttribute::WriteProtected), an Error exception will be
    * thrown.
    *
    * \ingroup view_properties
    */
   void DeleteProperty( const IsoString& property, bool notify = true );

   void DeleteProperty( const IsoString::ustring_base& property, bool notify = true )
   {
      DeleteProperty( IsoString( property ), notify );
   }

   /*!
    * Deletes the specified \a property and its associated value in this view,
    * only if it is currently defined. If \a property is not defined, calling
    * this member function has no effect.
    *
    * \ingroup view_properties
    */
   void DeletePropertyIfExists( const IsoString& property, bool notify = true )
   {
      if ( HasProperty( property ) )
         DeleteProperty( property, notify );
   }

   void DeletePropertyIfExists( const IsoString::ustring_base& property, bool notify = true )
   {
      DeletePropertyIfExists( IsoString( property ), notify );
   }

   /*!
    * Returns true iff the specified \a id string is a valid view identifier.
    *
    * A valid view identifier can include a preview separator (the sequence
    * "->") to separate between a main view identifier and a preview
    * identifier.
    */
   template <class S>
   static bool IsValidViewId( const S& id )
   {
      size_type p = id.Find( "->" );
      if ( p == String::notFound )
         return id.IsValidIdentifier();
      return id.Left( p ).IsValidIdentifier() && id.Substring( p+2 ).IsValidIdentifier();
   }

   /*!
    * Returns a view with the specified full identifier. If no view exists with
    * the specified identifier, this function returns View::Null().
    */
   static View ViewById( const IsoString& fullId );

   static View ViewById( const IsoString::ustring_base& fullId )
   {
      return ViewById( IsoString( fullId ) );
   }

   /*!
    * Returns a container with all the existing views. This includes all main
    * views and previews.
    */
   static Array<View> AllViews( bool excludePreviews = false );

   /*!
    * Returns a container with the existing previews. Main views are excluded.
    */
   static Array<View> AllPreviews();

protected:

   View( void* h ) : UIObject( h )
   {
   }

   View( const void* h ) : UIObject( h )
   {
   }

   View( std::nullptr_t ) : UIObject( nullptr )
   {
   }

   friend class ImageWindow;
   friend class ProcessImplementation; // for LaunchOn()
   friend class ProcessInterface;      // for event broadcasting functions
   friend class ProcessInstance;       // for ExecuteOn() and related functions
   friend class ViewList;
   friend class ViewListEventDispatcher;
   friend class ControlEventDispatcher;
   friend class ProcessContextDispatcher;
   friend class InterfaceDispatcher;
   friend class InternalViewEnumerator;
   friend class InternalPreviewEnumerator;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_View_h

// ----------------------------------------------------------------------------
// EOF pcl/View.h - Released 2024-12-28T16:53:48Z
