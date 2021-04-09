//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.2
// ----------------------------------------------------------------------------
// SampleFormatConversionInterface.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __SampleFormatConversionInterface_h
#define __SampleFormatConversionInterface_h

#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>

#include "SampleFormatConversionInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SampleFormatConversionInterface : public ProcessInterface
{
public:

   SampleFormatConversionInterface();
   virtual ~SampleFormatConversionInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void TrackViewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsImageNotifications() const override;
   void ImageCreated( const View& ) override;
   void ImageUpdated( const View& ) override;
   void ImageRenamed( const View& ) override;
   void ImageDeleted( const View& ) override;
   void ImageFocused( const View& ) override;
   void ImageLocked( const View& ) override;
   void ImageUnlocked( const View& ) override;

private:

   SampleFormatConversionInstance instance;

   struct GUIData
   {
      GUIData( SampleFormatConversionInterface& );

      void SetupTrackViewControls( SampleFormatConversionInterface&, bool );

      VerticalSizer     Global_Sizer;

      GroupBox          Conversion_GroupBox;
      VerticalSizer     Conversion_Sizer;
         RadioButton       Integer8Bit_RadioButton;
         RadioButton       Integer16Bit_RadioButton;
         RadioButton       Integer32Bit_RadioButton;
         RadioButton       FloatingPoint32Bit_RadioButton;
         RadioButton       FloatingPoint64Bit_RadioButton;

      GroupBox          CurrentViewInfo_GroupBox;
      HorizontalSizer   CurrentViewInfo_Sizer;
         Label             CurrentViewInfo_Label;
   };

   GUIData* GUI = nullptr;

   void UpdateConversionControls();
   void UpdateCurrentViewInfoControls();

   void __ButtonClick( Button& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SampleFormatConversionInterface* TheSampleFormatConversionInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SampleFormatConversionInterface_h

// ----------------------------------------------------------------------------
// EOF SampleFormatConversionInterface.h - Released 2021-04-09T19:41:48Z
