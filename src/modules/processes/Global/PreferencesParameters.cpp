//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.2
// ----------------------------------------------------------------------------
// PreferencesParameters.cpp - Released 2024-12-28T16:54:15Z
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

#include "PreferencesParameters.h"

#include <pcl/Font.h>
#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IMPLEMENT_STRING_METAPARAMETER    ( Application, styleSheetFile )
IMPLEMENT_STRING_METAPARAMETER    ( Application, resourceFile01 )
IMPLEMENT_STRING_METAPARAMETER    ( Application, resourceFile02 )
IMPLEMENT_STRING_METAPARAMETER    ( Application, resourceFile03 )
IMPLEMENT_STRING_METAPARAMETER    ( Application, resourceFile04 )
IMPLEMENT_STRING_METAPARAMETER    ( Application, resourceFile05 )
IMPLEMENT_STRING_METAPARAMETER    ( Application, resourceFile06 )
IMPLEMENT_STRING_METAPARAMETER    ( Application, resourceFile07 )
IMPLEMENT_STRING_METAPARAMETER    ( Application, resourceFile08 )
IMPLEMENT_STRING_METAPARAMETER    ( Application, resourceFile09 )
IMPLEMENT_STRING_METAPARAMETER    ( Application, resourceFile10 )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Application, autoUIScaling )
IMPLEMENT_DOUBLE_METAPARAMETER    ( Application, uiScalingFactor )
IMPLEMENT_INT32_METAPARAMETER     ( Application, fontResolution )
IMPLEMENT_STRING_METAPARAMETER    ( Application, lowResFont )
IMPLEMENT_STRING_METAPARAMETER    ( Application, highResFont )
IMPLEMENT_STRING_METAPARAMETER    ( Application, lowResMonoFont )
IMPLEMENT_STRING_METAPARAMETER    ( Application, highResMonoFont )
IMPLEMENT_INT32_METAPARAMETER     ( Application, messageBoxYesNoButtonOrder )
IMPLEMENT_INT32_METAPARAMETER     ( Application, messageBoxOkCancelButtonOrder )
IMPLEMENT_INT32_METAPARAMETER     ( Application, autoSaveSettingsPeriod )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Application, minifySettingsXML )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Application, compressSettingsBlocks )
IMPLEMENT_STRING_METAPARAMETER    ( Application, fundamentalEphemeridesFile )
IMPLEMENT_STRING_METAPARAMETER    ( Application, shortTermFundamentalEphemeridesFile )
IMPLEMENT_STRING_METAPARAMETER    ( Application, asteroidEphemeridesFile )
IMPLEMENT_STRING_METAPARAMETER    ( Application, shortTermAsteroidEphemeridesFile )
IMPLEMENT_STRING_METAPARAMETER    ( Application, kboEphemeridesFile )
IMPLEMENT_STRING_METAPARAMETER    ( Application, shortTermKBOEphemeridesFile )
IMPLEMENT_STRING_METAPARAMETER    ( Application, nutationModelFile )
IMPLEMENT_STRING_METAPARAMETER    ( Application, shortTermNutationModelFile )
IMPLEMENT_STRING_METAPARAMETER    ( Application, deltaTDataFile )
IMPLEMENT_STRING_METAPARAMETER    ( Application, deltaATDataFile )
IMPLEMENT_STRING_METAPARAMETER    ( Application, cipITRSDataFile )
IMPLEMENT_STRING_METAPARAMETER    ( Application, filtersDatabaseFile )
IMPLEMENT_STRING_METAPARAMETER    ( Application, whiteReferencesDatabaseFile )

// ----------------------------------------------------------------------------

IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  maximizeAtStartup )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  fullScreenAtStartup )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  showSplashAtStartup )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  checkForUpdatesAtStartup )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  confirmProgramTermination )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  acceptDroppedFiles )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  doubleClickLaunchesOpenDialog )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  hoverableAutoHideWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  desktopSettingsAware )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  nativeMenuBar )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  capitalizedMenuBars )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  windowButtonsOnTheLeft )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  translucentWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  translucentChildWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  fadeWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  fadeAutoHideWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  translucentAutoHideWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  fadeWorkspaces )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  fadeMenu )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  fadeToolTip )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  explodeIcons )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  implodeIcons )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  dropShadowChildWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  dropShadowIcons )
IMPLEMENT_DOUBLE_METAPARAMETER    ( MainWindow,  dropShadowBlurRadius )
IMPLEMENT_COLOR_METAPARAMETER     ( MainWindow,  dropShadowColor )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  animateWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  animateMenu )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  animateCombo )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  animateToolTip )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  animateToolBox )
IMPLEMENT_INT32_METAPARAMETER     ( MainWindow,  maxRecentFiles )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  showRecentlyUsed )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  showMostUsed )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  showFavorites )
IMPLEMENT_INT32_METAPARAMETER     ( MainWindow,  maxUsageListLength )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  expandRecentlyUsedAtStartup )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  expandMostUsedAtStartup )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  expandFavoritesAtStartup )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  openURLsWithInternalBrowser )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  openResourcesOnNewWebBrowserWindows )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  privateWebBrowsingMode )
IMPLEMENT_INT32_METAPARAMETER     ( MainWindow,  iconGridSpacing )
IMPLEMENT_STRING_METAPARAMETER    ( MainWindow,  wallpaperFile01 )
IMPLEMENT_STRING_METAPARAMETER    ( MainWindow,  wallpaperFile02 )
IMPLEMENT_STRING_METAPARAMETER    ( MainWindow,  wallpaperFile03 )
IMPLEMENT_STRING_METAPARAMETER    ( MainWindow,  wallpaperFile04 )
IMPLEMENT_STRING_METAPARAMETER    ( MainWindow,  wallpaperFile05 )
IMPLEMENT_STRING_METAPARAMETER    ( MainWindow,  wallpaperFile06 )
IMPLEMENT_STRING_METAPARAMETER    ( MainWindow,  wallpaperFile07 )
IMPLEMENT_STRING_METAPARAMETER    ( MainWindow,  wallpaperFile08 )
IMPLEMENT_STRING_METAPARAMETER    ( MainWindow,  wallpaperFile09 )
IMPLEMENT_STRING_METAPARAMETER    ( MainWindow,  wallpaperFile10 )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  useWallpapers )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( MainWindow,  highQualityWallpapers )

// ----------------------------------------------------------------------------

IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, backupFiles )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, defaultMasksShown )
IMPLEMENT_INT32_METAPARAMETER     ( ImageWindow, defaultMaskMode )
IMPLEMENT_INT32_METAPARAMETER     ( ImageWindow, defaultTransparencyMode )
IMPLEMENT_DOUBLE_METAPARAMETER    ( ImageWindow, defaultHorizontalResolution )
IMPLEMENT_DOUBLE_METAPARAMETER    ( ImageWindow, defaultVerticalResolution )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, defaultMetricResolution )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, defaultEmbedThumbnails )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, defaultEmbedProperties )
IMPLEMENT_STRING_METAPARAMETER    ( ImageWindow, defaultFileExtension )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, nativeFileDialogs )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, rememberFileOpenType )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, rememberFileSaveType )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, strictFileSaveMode )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, fileFormatWarnings )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, useFileNamesAsImageIdentifiers )
IMPLEMENT_INT32_METAPARAMETER     ( ImageWindow, cursorTolerance )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, zoomAtCursor )
IMPLEMENT_UINT32_METAPARAMETER    ( ImageWindow, wheelStepAngle )
IMPLEMENT_INT32_METAPARAMETER     ( ImageWindow, wheelDirection )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, touchEvents )
IMPLEMENT_DOUBLE_METAPARAMETER    ( ImageWindow, pinchSensitivity )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, fastScreenRenditions )
IMPLEMENT_INT32_METAPARAMETER     ( ImageWindow, fastScreenRenditionThreshold )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, highDPIRenditions );
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, default24BitScreenLUT )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, loadInitialProcessingFromCoreProperties )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, createPreviewsFromCoreProperties )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, loadAstrometricSolutions )
IMPLEMENT_TABLE_METAPARAMETER     ( ImageWindow, swapDirectories )
IMPLEMENT_ROW_STRING_METAPARAMETER( ImageWindow, swapDirectory )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, swapCompression )
IMPLEMENT_STRING_METAPARAMETER    ( ImageWindow, downloadsDirectory )
IMPLEMENT_STRING_METAPARAMETER    ( ImageWindow, proxyURL )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, followDownloadLocations )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, verboseNetworkOperations )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, showCaptionCurrentChannels )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, showCaptionZoomRatios )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, showCaptionIdentifiers )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, showCaptionFullPaths )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( ImageWindow, showActiveSTFIndicators )
IMPLEMENT_INT32_METAPARAMETER     ( ImageWindow, transparencyBrush )
IMPLEMENT_COLOR_METAPARAMETER     ( ImageWindow, transparencyBrushForegroundColor )
IMPLEMENT_COLOR_METAPARAMETER     ( ImageWindow, transparencyBrushBackgroundColor )
IMPLEMENT_COLOR_METAPARAMETER     ( ImageWindow, defaultTransparencyColor )

// ----------------------------------------------------------------------------

IMPLEMENT_STRING_METAPARAMETER    ( Identifiers, workspacePrefix )
IMPLEMENT_STRING_METAPARAMETER    ( Identifiers, imagePrefix )
IMPLEMENT_STRING_METAPARAMETER    ( Identifiers, previewPrefix )
IMPLEMENT_STRING_METAPARAMETER    ( Identifiers, processIconPrefix )
IMPLEMENT_STRING_METAPARAMETER    ( Identifiers, imageContainerIconPrefix )
IMPLEMENT_STRING_METAPARAMETER    ( Identifiers, newImageCaption )
IMPLEMENT_STRING_METAPARAMETER    ( Identifiers, clonePostfix )
IMPLEMENT_STRING_METAPARAMETER    ( Identifiers, noViewsAvailableText )
IMPLEMENT_STRING_METAPARAMETER    ( Identifiers, noViewSelectedText )
IMPLEMENT_STRING_METAPARAMETER    ( Identifiers, noPreviewsAvailableText )
IMPLEMENT_STRING_METAPARAMETER    ( Identifiers, noPreviewSelectedText )
IMPLEMENT_STRING_METAPARAMETER    ( Identifiers, brokenLinkText )

// ----------------------------------------------------------------------------

IMPLEMENT_BOOLEAN_METAPARAMETER   ( Process, enableParallelProcessing )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Process, enableParallelCoreRendering )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Process, enableParallelCoreColorManagement )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Process, enableParallelModuleProcessing )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Process, enableThreadCPUAffinity )
IMPLEMENT_INT32_METAPARAMETER     ( Process, maxModuleThreadPriority )
IMPLEMENT_INT32_METAPARAMETER     ( Process, maxProcessors )
IMPLEMENT_INT32_METAPARAMETER     ( Process, maxFileReadThreads )
IMPLEMENT_INT32_METAPARAMETER     ( Process, maxFileWriteThreads )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Process, enableCUDAAcceleration )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Process, initCUDARuntimeAtStartup )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Process, backupFiles )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Process, generateScriptComments )
IMPLEMENT_INT32_METAPARAMETER     ( Process, maxConsoleLines )
IMPLEMENT_INT32_METAPARAMETER     ( Process, consoleDelay )
IMPLEMENT_INT32_METAPARAMETER     ( Process, autoSavePSMPeriod )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Process, alertOnProcessCompleted )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Process, enableExecutionStatistics )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Process, enableLaunchStatistics )

// ----------------------------------------------------------------------------

IMPLEMENT_BOOLEAN_METAPARAMETER   ( Security, allowUnsignedScriptExecution )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Security, allowUnsignedModuleInstallation )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Security, allowUnsignedRepositories )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Security, allowInsecureRepositories )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Security, reportScriptSignatures )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Security, reportModuleSignatures )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Security, warnOnUnsignedCodeExecution )
IMPLEMENT_BOOLEAN_METAPARAMETER   ( Security, enableLocalSigningIdentity )

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PreferencesParameters.cpp - Released 2024-12-28T16:54:15Z
