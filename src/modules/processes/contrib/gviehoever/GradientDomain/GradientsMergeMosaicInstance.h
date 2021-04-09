//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 0.6.4
// ----------------------------------------------------------------------------
// GradientsMergeMosaicInstance.h - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2020. Licensed under LGPL 2.1
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ----------------------------------------------------------------------------

#ifndef __GradientsMergeMosaicInstance_h
#define __GradientsMergeMosaicInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Vector.h>

#include "GradientsMergeMosaic.h" //for image types

namespace pcl
{

// ----------------------------------------------------------------------------

class GradientsMergeMosaicInstance : public ProcessImplementation
{
public:

   GradientsMergeMosaicInstance( const MetaProcess* );
   GradientsMergeMosaicInstance( const GradientsMergeMosaicInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool IsHistoryUpdater( const View& v ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   /// create image window that has rImage as contents
   static ImageWindow CreateImageWindow( const IsoString& id, GradientsMergeMosaic::imageType_t const& rImage_p );

   struct ImageItem
   {
      pcl_bool enabled = true; // if disabled, skip (ignore) this image
      String path;             // absolute file path

      ImageItem( const String& p = String() )
         : path( p )
      {
      }

      ImageItem( const ImageItem& ) = default;

      bool IsValid() const
      {
         return !enabled || !path.IsEmpty();
      }
   };

   typedef Array<ImageItem> image_list;

   /// The set of target frames to be calibrated
   image_list targetFrames;

   /// type of overlay operation
   pcl_enum type;

   /// the radius in pixels of an erosion of the image borders
   int32 shrinkCount;

   /// the radius in pixels of a convolution of the image borders
   int32 featherRadius;

   /// black point value. Pixel Values < blackPoint are considered transparent
   double blackPoint;

   /// true if we need to generate a mask view
   pcl_bool generateMask;

   friend class GradientsMergeMosaicInterface;
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif

// ----------------------------------------------------------------------------
// EOF GradientsMergeMosaicInstance.h - Released 2021-04-09T19:41:49Z
