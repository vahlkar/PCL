//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard StarNet Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// StarNetProcess.h - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarNet PixInsight module.
//
// Copyright (c) 2018-2020 Nikita Misiura
//
// This software is available under Attribution-NonCommercial-ShareAlike 4.0
// International Creative Commons license (CC BY-NC-SA 4.0):
// 
// https://creativecommons.org/licenses/by-nc-sa/4.0/
// 
// In short: You are free to use and redistribute the code in any medium or
// format, but only under the same license terms. You can transform and build
// your projects upon it. You can NOT use the code for commercial purposes. You
// must give appropriate credit for usage of the code.
// 
// This product is based on software from the PixInsight project, developed by
// Pleiades Astrophoto and its contributors:
// 
// https://pixinsight.com/
// ----------------------------------------------------------------------------

#ifndef __StarNetProcess_h
#define __StarNetProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class StarNetProcess : public MetaProcess
{
public:

   StarNetProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   uint32 Version() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;

   void SetRGBWeightsFilePath( const String& );
   const String& RGBWeightsFilePath() const;
   bool IsValidRGBWeightsFilePath() const;

   void SetGrayscaleWeightsFilePath( const String& );
   const String& GrayscaleWeightsFilePath() const;
   bool IsValidGrayscaleWeightsFilePath() const;

   bool PreferencesLoaded() const;
   void LoadPreferences();
   void SavePreferences();
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern StarNetProcess* TheStarNetProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StarNetProcess_h

// ----------------------------------------------------------------------------
// EOF StarNetProcess.h - Released 2021-04-09T19:41:49Z
