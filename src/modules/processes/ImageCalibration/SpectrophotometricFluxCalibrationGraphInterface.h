//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 2.2.7
// ----------------------------------------------------------------------------
// SpectrophotometricFluxCalibrationGraphInterface.h - Released 2024-12-17T18:15:44Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#ifndef __SpectrophotometricFluxCalibrationGraphInterface_h
#define __SpectrophotometricFluxCalibrationGraphInterface_h

#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/WebView.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class LinearFit;

class SpectrophotometricFluxCalibrationGraphInterface : public ProcessInterface
{
public:

   SpectrophotometricFluxCalibrationGraphInterface();
   ~SpectrophotometricFluxCalibrationGraphInterface() override;

   IsoString Id() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned&/*flags*/ ) override;
   MetaProcess* Process() const override;

   using ProcessInterface::Launch;

   void UpdateGraphs( const String& viewId, const String& catalogName,
                      const String& filterNameK, const String& filterNameR, const String& filterNameG, const String& filterNameB,
                      const String& qeCurveName,
                      const Vector& samplesRK, const Vector& samplesG, const Vector& samplesB,
                      const Vector& scaleFactors, const Vector& sigmas );

   void CleanUp();

   void ExportPDF() const;
   void ToggleText() const;
   void ToggleHelp() const;

private:

   StringList m_htmlFiles;

   struct GUIData
   {
      GUIData( SpectrophotometricFluxCalibrationGraphInterface& );

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

extern SpectrophotometricFluxCalibrationGraphInterface* TheSpectrophotometricFluxCalibrationGraphInterface;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SpectrophotometricFluxCalibrationGraphInterface_h

// ----------------------------------------------------------------------------
// EOF SpectrophotometricFluxCalibrationGraphInterface.h - Released 2024-12-17T18:15:44Z
