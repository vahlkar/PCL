//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.6.0
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// ColorCalibrationGraphInterface.h - Released 2023-09-14T17:02:22Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __ColorCalibrationGraphInterface_h
#define __ColorCalibrationGraphInterface_h

#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/WebView.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class LinearFit;

class ColorCalibrationGraphInterface : public ProcessInterface
{
public:

   ColorCalibrationGraphInterface();
   ~ColorCalibrationGraphInterface() override;

   IsoString Id() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned&/*flags*/ ) override;

   using ProcessInterface::Launch;

   virtual MetaProcess* Process() const override = 0; // redeclared as pure virtual
   virtual bool InMagnitudeUnits() const = 0;

   void UpdateGraphs( const String& viewId, const String& catalogName,
                      const String& filterNameR, const String& filterNameG, const String& filterNameB,
                      const String& qeCurveName, const String& whiteRefName,
                      const Vector& catRG, const Vector& imgRG, const LinearFit& fitRG,
                      const Vector& catBG, const Vector& imgBG, const LinearFit& fitBG,
                      const Vector& wbFactors,
                      float catWR, float catWB );

   void CleanUp();

   void ExportPDF() const;
   void ToggleText() const;
   void ToggleHelp() const;

private:

   StringList m_htmlFiles;

   struct GUIData
   {
      GUIData( ColorCalibrationGraphInterface& );

      VerticalSizer     Global_Sizer;
         HorizontalSizer   Buttons_Sizer;
            ToolButton        ExportPDF_ToolButton;
            ToolButton        ToggleText_ToolButton;
            ToolButton        ToggleHelp_ToolButton;
         WebView           Graph_WebView;
   };

   GUIData* GUI = nullptr;

   void e_LoadFinished( WebView& sender, bool state );
   void e_Click( Button& sender, bool checked );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ColorCalibrationGraphInterface_h

// ----------------------------------------------------------------------------
// EOF ColorCalibrationGraphInterface.h - Released 2023-09-14T17:02:22Z
