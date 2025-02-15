//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 1.2.1
// ----------------------------------------------------------------------------
// FITSPreferencesDialog.h - Released 2024-12-28T16:54:05Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
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

#ifndef __FITSPreferencesDialog_h
#define __FITSPreferencesDialog_h

#include "FITSOptionsDialog.h"
#include "FITSFormat.h"

#include <pcl/Dialog.h>
#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/PushButton.h>
#include <pcl/CheckBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ComboBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FITSPreferencesDialog: public Dialog
{
public:

   FITSPreferencesDialog( const FITSFormat::OutOfRangePolicyOptions&, const FITSImageOptions& );

   FITSFormat::OutOfRangePolicyOptions outOfRange;
   FITSImageOptions                    fitsOptions;

private:

   VerticalSizer     Global_Sizer;

   GroupBox          ReadRange_GroupBox;
   VerticalSizer     ReadRange_Sizer;
      NumericEdit       LowerRange_NumericEdit;
      NumericEdit       UpperRange_NumericEdit;

   GroupBox          ReadRangeOptions_GroupBox;
   VerticalSizer     ReadRangeOptions_Sizer;
      HorizontalSizer   ReadRangeMode_Sizer;
         Label             ReadRangeMode_Label;
         ComboBox          ReadRangeMode_ComboBox;
      HorizontalSizer   ReadRescaleMode_Sizer;
         Label             ReadRescaleMode_Label;
         ComboBox          ReadRescaleMode_ComboBox;

   GroupBox          MiscellaneousOptions_GroupBox;
   VerticalSizer     MiscellaneousOptions_Sizer;
      HorizontalSizer   CoordinateOrigin_Sizer;
         Label             CoordinateOrigin_Label;
         ComboBox          CoordinateOrigin_ComboBox;
      HorizontalSizer   IntegerSignedness_Sizer;
         Label             IntegerSignedness_Label;
         ComboBox          IntegerSignedness_ComboBox;
      HorizontalSizer   UseRowOrderKeywords_Sizer;
         CheckBox          UseRowOrderKeywords_CheckBox;
      HorizontalSizer   WriteScalingKeywordsForSignedData_Sizer;
         CheckBox          WriteScalingKeywordsForSignedData_CheckBox;
      HorizontalSizer   SignedIntegersArePhysical_Sizer;
         CheckBox          SignedIntegersArePhysical_CheckBox;
      HorizontalSizer   CleanupHeaders_Sizer;
         CheckBox          CleanupHeaders_CheckBox;

   HorizontalSizer   BottomSection_Sizer;
      PushButton        Reset_PushButton;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FITSPreferencesDialog_h

// ----------------------------------------------------------------------------
// EOF FITSPreferencesDialog.h - Released 2024-12-28T16:54:05Z
