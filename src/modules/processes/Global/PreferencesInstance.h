//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.2
// ----------------------------------------------------------------------------
// PreferencesInstance.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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

#ifndef __PreferencesInstance_h
#define __PreferencesInstance_h

#include <pcl/MetaParameter.h> // pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct ApplicationPreferences
{
   String      styleSheetFile;
   String      resourceFile01;
   String      resourceFile02;
   String      resourceFile03;
   String      resourceFile04;
   String      resourceFile05;
   String      resourceFile06;
   String      resourceFile07;
   String      resourceFile08;
   String      resourceFile09;
   String      resourceFile10;
   pcl_bool    autoUIScaling;
   double      uiScalingFactor; // in [1,4]
   int32       fontResolution;  // in dpi
   String      lowResFont;
   String      highResFont;
   String      lowResMonoFont;
   String      highResMonoFont;
   int32       messageBoxYesNoButtonOrder;    // 0 = default, 1 = Yes/No, 2 = No/Yes
   int32       messageBoxOkCancelButtonOrder; // 0 = default, 1 = OK/Cancel, 2 = Cancel/OK
   int32       autoSaveSettingsPeriod; // seconds, <= 0 : disabled, >= 30 otherwise
   pcl_bool    minifySettingsXML;
   pcl_bool    compressSettingsBlocks;
   String      fundamentalEphemeridesFile;
   String      shortTermFundamentalEphemeridesFile;
   String      asteroidEphemeridesFile;
   String      shortTermAsteroidEphemeridesFile;
   String      kboEphemeridesFile;
   String      shortTermKBOEphemeridesFile;
   String      nutationModelFile;
   String      shortTermNutationModelFile;
   String      deltaTDataFile;
   String      deltaATDataFile;
   String      cipITRSDataFile;
   String      filtersDatabaseFile;
   String      whiteReferencesDatabaseFile;
};

// ----------------------------------------------------------------------------

struct MainWindowPreferences
{
   pcl_bool    maximizeAtStartup;
   pcl_bool    fullScreenAtStartup;
   pcl_bool    showSplashAtStartup;
   pcl_bool    checkForUpdatesAtStartup;
   pcl_bool    confirmProgramTermination;
   pcl_bool    acceptDroppedFiles;
   pcl_bool    doubleClickLaunchesOpenDialog;
   pcl_bool    hoverableAutoHideWindows;
   pcl_bool    desktopSettingsAware;
   pcl_bool    nativeMenuBar; // OS X only
   pcl_bool    capitalizedMenuBars;
   pcl_bool    windowButtonsOnTheLeft; // true by default on OS X
   pcl_bool    translucentWindows;
   pcl_bool    translucentChildWindows;
   pcl_bool    fadeWindows;
   pcl_bool    fadeAutoHideWindows;
   pcl_bool    translucentAutoHideWindows;
   pcl_bool    fadeWorkspaces;
   pcl_bool    fadeMenu;
   pcl_bool    fadeToolTip;
   pcl_bool    explodeIcons;
   pcl_bool    implodeIcons;
   pcl_bool    dropShadowChildWindows;
   pcl_bool    dropShadowIcons;
   double      dropShadowBlurRadius;
   uint32      dropShadowColor;
   pcl_bool    animateWindows;
   pcl_bool    animateMenu;
   pcl_bool    animateCombo;
   pcl_bool    animateToolTip;
   pcl_bool    animateToolBox;
   int32       maxRecentFiles;
   pcl_bool    showRecentlyUsed;
   pcl_bool    showMostUsed;
   pcl_bool    showFavorites;
   int32       maxUsageListLength;
   pcl_bool    expandRecentlyUsedAtStartup;
   pcl_bool    expandMostUsedAtStartup;
   pcl_bool    expandFavoritesAtStartup;
   pcl_bool    openURLsWithInternalBrowser;
   pcl_bool    openResourcesOnNewWebBrowserWindows;
   pcl_bool    privateWebBrowsingMode;
   int32       iconGridSpacing;
   String      wallpaperFile01;
   String      wallpaperFile02;
   String      wallpaperFile03;
   String      wallpaperFile04;
   String      wallpaperFile05;
   String      wallpaperFile06;
   String      wallpaperFile07;
   String      wallpaperFile08;
   String      wallpaperFile09;
   String      wallpaperFile10;
   pcl_bool    useWallpapers;
   pcl_bool    highQualityWallpapers;
};

// ----------------------------------------------------------------------------

struct ImageWindowPreferences
{
   pcl_bool    backupFiles;
   pcl_bool    defaultMasksShown;
   int32       defaultMaskMode;
   int32       defaultTransparencyMode;
   double      defaultHorizontalResolution;
   double      defaultVerticalResolution;
   pcl_bool    defaultMetricResolution;
   pcl_bool    defaultEmbedThumbnails;
   pcl_bool    defaultEmbedProperties;
   String      defaultFileExtension;
   pcl_bool    nativeFileDialogs;
   pcl_bool    rememberFileOpenType;
   pcl_bool    rememberFileSaveType;
   pcl_bool    strictFileSaveMode;
   pcl_bool    fileFormatWarnings;
   pcl_bool    useFileNamesAsImageIdentifiers;
   int32       cursorTolerance;
   pcl_bool    zoomAtCursor;
   uint32      wheelStepAngle; // in degrees
   int         wheelDirection; // >= 0 forward is zoom out | < 0 forward is zoom in
   pcl_bool    touchEvents;
   double      pinchSensitivity;
   pcl_bool    fastScreenRenditions;
   int32       fastScreenRenditionThreshold;
   pcl_bool    highDPIRenditions;
   pcl_bool    default24BitScreenLUT;
   pcl_bool    loadInitialProcessingFromCoreProperties;
   pcl_bool    createPreviewsFromCoreProperties;
   pcl_bool    loadAstrometricSolutions;
   StringList  swapDirectories;
   pcl_bool    swapCompression;
   String      downloadsDirectory;
   String      proxyURL;
   pcl_bool    followDownloadLocations;
   pcl_bool    verboseNetworkOperations;
   pcl_bool    showCaptionCurrentChannels;
   pcl_bool    showCaptionZoomRatios;
   pcl_bool    showCaptionIdentifiers;
   pcl_bool    showCaptionFullPaths;
   pcl_bool    showActiveSTFIndicators;
   int32       transparencyBrush; // corresponds to pcl::BackgroundBrush::value_type
   uint32      transparencyBrushForegroundColor;
   uint32      transparencyBrushBackgroundColor;
   uint32      defaultTransparencyColor;
};

// ----------------------------------------------------------------------------

struct IdentifiersPreferences
{
   String      workspacePrefix;
   String      imagePrefix;
   String      previewPrefix;
   String      processIconPrefix;
   String      imageContainerIconPrefix;
   String      newImageCaption;
   String      clonePostfix;
   String      noViewsAvailableText;
   String      noViewSelectedText;
   String      noPreviewsAvailableText;
   String      noPreviewSelectedText;
   String      brokenLinkText;
};

// ----------------------------------------------------------------------------

struct ProcessPreferences
{
   pcl_bool    enableParallelProcessing;
   pcl_bool    enableParallelCoreRendering;
   pcl_bool    enableParallelCoreColorManagement;
   pcl_bool    enableParallelModuleProcessing;
   pcl_bool    enableThreadCPUAffinity;
   int32       maxModuleThreadPriority;
   int32       maxProcessors;
   int32       maxFileReadThreads;
   int32       maxFileWriteThreads;
   pcl_bool    enableCUDAAcceleration;
   pcl_bool    initCUDARuntimeAtStartup;
   pcl_bool    backupFiles; // for PSM files
   pcl_bool    generateScriptComments;
   int32       maxConsoleLines;
   int32       consoleDelay;  // in ms
   int32       autoSavePSMPeriod; // seconds, <= 0 : disabled, >= 30 otherwise
   pcl_bool    alertOnProcessCompleted;
   pcl_bool    enableExecutionStatistics;
   pcl_bool    enableLaunchStatistics;
};

// ----------------------------------------------------------------------------

struct SecurityPreferences
{
   pcl_bool    allowUnsignedScriptExecution;
   pcl_bool    allowUnsignedModuleInstallation; // ### read-only since core version 1.9.0
   pcl_bool    allowUnsignedRepositories;
   pcl_bool    allowInsecureRepositories;
   pcl_bool    reportScriptSignatures;
   pcl_bool    reportModuleSignatures;
   pcl_bool    warnOnUnsignedCodeExecution;
   pcl_bool    enableLocalSigningIdentity;
};

// ----------------------------------------------------------------------------

class PreferencesInstance : public ProcessImplementation
{
public:

   PreferencesInstance( const MetaProcess* );
   PreferencesInstance( const PreferencesInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool CanExecuteGlobal( pcl::String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   void LoadDefaultSettings();
   void LoadCurrentSettings();

private:

   ApplicationPreferences application;
   MainWindowPreferences  mainWindow;
   ImageWindowPreferences imageWindow;
   IdentifiersPreferences identifiers;
   ProcessPreferences     process;
   SecurityPreferences    security;

   String* StringParameterFromMetaParameter( const MetaParameter* );

   friend class PreferencesProcess;
   friend class PreferencesInterface;

   friend class DefaultImageResolutionPreferencesPage;
   friend class DefaultMaskSettingsPreferencesPage;
   friend class DefaultTransparencySettingsPreferencesPage;
   friend class DirectoriesAndNetworkPreferencesPage;
   friend class EphemeridesPreferencesPage;
   friend class FiltersPreferencesPage;
   friend class FileIOPreferencesPage;
   friend class GUIEffectsPreferencesPage;
   friend class MessageBoxPreferencesPage;
   friend class IdentifiersPreferencesPage;
   friend class MainWindowPreferencesPage;
   friend class MiscImageWindowSettingsPreferencesPage;
   friend class MiscProcessingPreferencesPage;
   friend class ParallelProcessingPreferencesPage;
   friend class ResourcesPreferencesPage;
   friend class SecurityPreferencesPage;
   friend class SettingsAndCacheDataPreferencesPage;
   friend class StringsPreferencesPage;
   friend class WallpapersPreferencesPage;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PreferencesInstance_h

// ----------------------------------------------------------------------------
// EOF PreferencesInstance.h - Released 2024-12-28T16:54:15Z
