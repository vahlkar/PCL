//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/GlobalSettings.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_GlobalSettings_h
#define __PCL_GlobalSettings_h

/// \file pcl/GlobalSettings.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Color.h>
#include <pcl/String.h>

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
#include <pcl/Font.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::GlobalVariableType
 * \brief Data types for global platform variables
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>GlobalVariableType::Undefined</td>  <td>Indicates that the requested global variable does not exist.</td></tr>
 * <tr><td>GlobalVariableType::Flag</td>       <td>Boolean value.</td></tr>
 * <tr><td>GlobalVariableType::Integer</td>    <td>Signed integer.</td></tr>
 * <tr><td>GlobalVariableType::Unsigned</td>   <td>Unsigned integer.</td></tr>
 * <tr><td>GlobalVariableType::Real</td>       <td>Floating point real (IEEE 64-bit floating point).</td></tr>
 * <tr><td>GlobalVariableType::Color</td>      <td>RGBA color (uint32).</td></tr>
 * <tr><td>GlobalVariableType::Font</td>       <td>A font face (a string).</td></tr>
 * <tr><td>GlobalVariableType::String</td>     <td>A UTF-16 string.</td></tr>
 * </table>
 */
namespace GlobalVariableType
{
   enum value_type
   {
      Undefined,  // The requested global variable doesn't exist
      Flag,       // bool
      Integer,    // int
      Unsigned,   // unsigned
      Real,       // double
      Color,      // RGBA (= uint32)
      Font,       // Font
      String      // String
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class PixInsightSettings
 * \brief Retrieves global settings from the PixInsight core application
 *
 * PixInsight global settings, also known as <em>global variables</em>, are
 * identifier/value pairs available from the core application to all installed
 * modules. Settings can be of six types: flags (or Boolean), integer (signed
 * and unsigned), real (floating point), color, font, and string. See the
 * pcl::GlobalVariableType namespace for more details.
 *
 * Below is a complete list of all global settings available in current
 * versions of the PixInsight platform (up-to-date as of core version 1.9.0
 * Lockhart, released November 2024).
 *
 * <h3>PixInsight Public Global Variables</h3>
 *
 * <h4>Global %Flags</h4>
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>Application/AutoUIScaling</td><td></td></tr>
 * <tr><td>Application/CompressSettingsBlocks</td><td></td></tr>
 * <tr><td>Application/HealRegistryInformationOnUpdates</td><td></td></tr>
 * <tr><td>Application/MinifySettingsXML</td><td></td></tr>
 * <tr><td>ColorManagement/DefaultEmbedProfilesInGrayscaleImages</td><td></td></tr>
 * <tr><td>ColorManagement/DefaultEmbedProfilesInRGBImages</td><td></td></tr>
 * <tr><td>ColorManagement/DefaultGamutCheckEnabled</td><td></td></tr>
 * <tr><td>ColorManagement/DefaultProofingEnabled</td><td></td></tr>
 * <tr><td>ColorManagement/IsEnabled</td><td></td></tr>
 * <tr><td>ColorManagement/IsValid</td><td>Read-only.</td></tr>
 * <tr><td>ColorManagement/UseLowResolutionCLUTs</td><td></td></tr>
 * <tr><td>ColorManagement/UseProofingBPC</td><td></td></tr>
 * <tr><td>ImageWindow/BackupFiles</td><td></td></tr>
 * <tr><td>ImageWindow/CreatePreviewsFromCoreProperties</td><td></td></tr>
 * <tr><td>ImageWindow/Default24BitScreenLUT</td><td></td></tr>
 * <tr><td>ImageWindow/DefaultEmbedProperties</td><td></td></tr>
 * <tr><td>ImageWindow/DefaultEmbedThumbnails</td><td></td></tr>
 * <tr><td>ImageWindow/DefaultMasksShown</td><td></td></tr>
 * <tr><td>ImageWindow/DefaultMetricResolution</td><td></td></tr>
 * <tr><td>ImageWindow/FastScreenRenditions</td><td></td></tr>
 * <tr><td>ImageWindow/FileFormatWarnings</td><td></td></tr>
 * <tr><td>ImageWindow/FollowDownloadLocations</td><td></td></tr>
 * <tr><td>ImageWindow/HighDPIRenditions</td><td></td></tr>
 * <tr><td>ImageWindow/LoadAstrometricSolutions</td><td></td></tr>
 * <tr><td>ImageWindow/LoadInitialProcessingFromCoreProperties</td><td></td></tr>
 * <tr><td>ImageWindow/MeasureScreenRenderingPerformance</td><td></td></tr>
 * <tr><td>ImageWindow/NativeFileDialogs</td><td></td></tr>
 * <tr><td>ImageWindow/ProjectVerifyIncrementalChecksums</td><td></td></tr>
 * <tr><td>ImageWindow/RememberFileOpenType</td><td></td></tr>
 * <tr><td>ImageWindow/RememberFileSaveType</td><td></td></tr>
 * <tr><td>ImageWindow/ShowActiveSTFIndicators</td><td></td></tr>
 * <tr><td>ImageWindow/ShowCaptionCurrentChannels</td><td></td></tr>
 * <tr><td>ImageWindow/ShowCaptionFullPaths</td><td></td></tr>
 * <tr><td>ImageWindow/ShowCaptionIdentifiers</td><td></td></tr>
 * <tr><td>ImageWindow/ShowCaptionZoomRatios</td><td></td></tr>
 * <tr><td>ImageWindow/ShowSynchronizationIndicators</td><td></td></tr>
 * <tr><td>ImageWindow/ShowSynchronizationVectors</td><td></td></tr>
 * <tr><td>ImageWindow/ShowViewSelectorImageThumbnails</td><td></td></tr>
 * <tr><td>ImageWindow/StrictFileSaveMode</td><td></td></tr>
 * <tr><td>ImageWindow/SwapCompression</td><td></td></tr>
 * <tr><td>ImageWindow/TouchEvents</td><td></td></tr>
 * <tr><td>ImageWindow/UseFileNamesAsImageIdentifiers</td><td></td></tr>
 * <tr><td>ImageWindow/VerboseNetworkOperations</td><td>Ignored on Windows.</td></tr>
 * <tr><td>ImageWindow/ZoomAtCursor</td><td></td></tr>
 * <tr><td>MainWindow/AcceptDroppedFiles</td><td></td></tr>
 * <tr><td>MainWindow/AnimateCombo</td><td></td></tr>
 * <tr><td>MainWindow/AnimateMenu</td><td></td></tr>
 * <tr><td>MainWindow/AnimateToolBox</td><td></td></tr>
 * <tr><td>MainWindow/AnimateToolTip</td><td></td></tr>
 * <tr><td>MainWindow/AnimateWindows</td><td></td></tr>
 * <tr><td>MainWindow/CapitalizedMenuBars</td><td></td></tr>
 * <tr><td>MainWindow/CheckForUpdatesAtStartup</td><td></td></tr>
 * <tr><td>MainWindow/ConfirmProgramTermination</td><td></td></tr>
 * <tr><td>MainWindow/DesktopSettingsAware</td><td></td></tr>
 * <tr><td>MainWindow/DoubleClickLaunchesOpenDialog</td><td></td></tr>
 * <tr><td>MainWindow/DropShadowChildWindows</td><td></td></tr>
 * <tr><td>MainWindow/DropShadowIcons</td><td></td></tr>
 * <tr><td>MainWindow/ExpandFavoritesAtStartup</td><td></td></tr>
 * <tr><td>MainWindow/ExpandMostUsedAtStartup</td><td></td></tr>
 * <tr><td>MainWindow/ExpandRecentlyUsedAtStartup</td><td></td></tr>
 * <tr><td>MainWindow/ExplodeIcons</td><td></td></tr>
 * <tr><td>MainWindow/FadeAutoHideWindows</td><td></td></tr>
 * <tr><td>MainWindow/FadeMenu</td><td></td></tr>
 * <tr><td>MainWindow/FadeToolTip</td><td></td></tr>
 * <tr><td>MainWindow/FadeWindows</td><td></td></tr>
 * <tr><td>MainWindow/FadeWorkspaces</td><td></td></tr>
 * <tr><td>MainWindow/FullScreenAtStartup</td><td></td></tr>
 * <tr><td>MainWindow/HighQualityWallpapers</td><td></td></tr>
 * <tr><td>MainWindow/HoverableAutoHideWindows</td><td></td></tr>
 * <tr><td>MainWindow/ImplodeIcons</td><td></td></tr>
 * <tr><td>MainWindow/MaximizeAtStartup</td><td></td></tr>
 * <tr><td>MainWindow/NativeMenuBar</td><td></td></tr>
 * <tr><td>MainWindow/OpenURLsWithInternalBrowser</td><td></td></tr>
 * <tr><td>MainWindow/OpenResourcesOnNewWebBrowserWindows</td><td></td></tr>
 * <tr><td>MainWindow/PrivateWebBrowsingMode</td><td></td></tr>
 * <tr><td>MainWindow/ShowFavorites</td><td></td></tr>
 * <tr><td>MainWindow/ShowMostUsed</td><td></td></tr>
 * <tr><td>MainWindow/ShowRecentlyUsed</td><td></td></tr>
 * <tr><td>MainWindow/ShowSplashAtStartup</td><td></td></tr>
 * <tr><td>MainWindow/ShowViewListImageThumbnails</td><td></td></tr>
 * <tr><td>MainWindow/ShowWorkspaceThumbnails</td><td></td></tr>
 * <tr><td>MainWindow/TranslucentAutoHideWindows</td><td></td></tr>
 * <tr><td>MainWindow/TranslucentChildWindows</td><td></td></tr>
 * <tr><td>MainWindow/TranslucentWindows</td><td></td></tr>
 * <tr><td>MainWindow/UseWallpapers</td><td></td></tr>
 * <tr><td>MainWindow/WindowButtonsOnTheLeft</td><td></td></tr>
 * <tr><td>Process/AlertOnProcessCompleted</td><td></td></tr>
 * <tr><td>Process/BackupFiles</td><td></td></tr>
 * <tr><td>Process/EnableCUDAAcceleration</td><td></td></tr>
 * <tr><td>Process/EnableExecutionStatistics</td><td></td></tr>
 * <tr><td>Process/EnableLaunchStatistics</td><td></td></tr>
 * <tr><td>Process/EnableParallelCoreColorManagement</td><td></td></tr>
 * <tr><td>Process/EnableParallelCoreRendering</td><td></td></tr>
 * <tr><td>Process/EnableParallelModuleProcessing</td><td></td></tr>
 * <tr><td>Process/EnableParallelProcessing</td><td></td></tr>
 * <tr><td>Process/EnableThreadCPUAffinity</td><td></td></tr>
 * <tr><td>Process/GenerateScriptComments</td><td></td></tr>
 * <tr><td>Process/InitCUDARuntimeAtStartup</td><td></td></tr>
 * <tr><td>Process/VerifyScriptChecksums</td><td></td></tr>
 * <tr><td>Security/AllowInsecureRepositories</td><td></td></tr>
 * <tr><td>Security/AllowUnsignedModuleInstallation</td><td></td></tr>
 * <tr><td>Security/AllowUnsignedRepositories</td><td></td></tr>
 * <tr><td>Security/AllowUnsignedScriptExecution</td><td></td></tr>
 * <tr><td>Security/EnableLocalSigningIdentity</td><td></td></tr>
 * <tr><td>Security/ReportModuleSignatures</td><td></td></tr>
 * <tr><td>Security/ReportScriptSignatures</td><td></td></tr>
 * <tr><td>Security/WarnOnUnsignedCodeExecution</td><td></td></tr>
 * </table>
 *
 * <h4>Global Integers</h4>
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>Application/AutoSaveSettingsPeriod</td><td>In seconds.</td></tr>
 * <tr><td>Application/FontResolution</td><td>In dots per inch.</td></tr>
 * <tr><td>Application/Instance</td><td>The running application's instance number (&ge; 1).</td></tr>
 * <tr><td>Application/MessageBoxOkCancelButtonOrder</td><td>0 = default, 1 = Ok/Cancel, 2 = Cancel/Ok</td></tr>
 * <tr><td>Application/MessageBoxYesNoButtonOrder</td><td>0 = default, 1 = Yes/No, 2 = No/Yes</td></tr>
 * <tr><td>Application/SurfaceSwapInterval</td><td>The minimum number of video frames displayed before buffer swaps (vertical synchronization). Read-only.</td></tr>
 * <tr><td>ColorManagement/DefaultRenderingIntent</td><td>See the pcl::ICCRenderingIntent namespace.</td></tr>
 * <tr><td>ColorManagement/OnMissingProfile</td><td>Core application policies. See the documentation for ColorManagementSetup.</td></tr>
 * <tr><td>ColorManagement/OnProfileMismatch</td><td>Core application policies. See the documentation for ColorManagementSetup.</td></tr>
 * <tr><td>ColorManagement/ProofingIntent</td><td>See the pcl::ICCRenderingIntent namespace.</td></tr>
 * <tr><td>ImageWindow/CursorTolerance</td><td>In device pixels.</td></tr>
 * <tr><td>ImageWindow/DefaultMaskMode</td><td>See the pcl::MaskMode namespace.</td></tr>
 * <tr><td>ImageWindow/DefaultTransparencyMode</td><td>See the pcl::TransparencyMode namespace.</td></tr>
 * <tr><td>ImageWindow/FastScreenRenditionThreshold</td><td>In MiB.</td></tr>
 * <tr><td>ImageWindow/ImageThumbnailSize</td><td>In image pixels.</td></tr>
 * <tr><td>ImageWindow/ProjectThumbnailSize</td><td>In image pixels.</td></tr>
 * <tr><td>ImageWindow/WheelStepAngle</td><td>In degrees, unsigned.</td></tr>
 * <tr><td>ImageWindow/WheelDirection</td><td>When &ge; 0, rotating forward zooms out. When &lt; 0, rotating forward zooms in.</td></tr>
 * <tr><td>MainWindow/MaxRecentFiles</td><td>Maximum length of recent file menu lists.</td></tr>
 * <tr><td>Process/AutoSavePSMPeriod</td><td>In seconds.</td></tr>
 * <tr><td>Process/ConsoleCommandTruncationLimit</td><td>In characters.</td></tr>
 * <tr><td>Process/ConsoleDelay</td><td>In milliseconds.</td></tr>
 * <tr><td>Process/MaxConsoleLines</td><td>Maximum number of stored text lines on %Process %Console.</td></tr>
 * <tr><td>Process/MaxModuleThreadPriority</td><td>From 0=idle to 7=real-time.</td></tr>
 * <tr><td>Process/MaxProcessors</td><td>Maximum number of processor cores allowed for installed modules.</td></tr>
 * <tr><td>Process/MaxFileReadThreads</td><td>Maximum number of concurrent file reading threads.</td></tr>
 * <tr><td>Process/MaxFileWriteThreads</td><td>Maximum number of concurrent file writing threads.</td></tr>
 * <tr><td>Process/MaxUsageListLength</td><td>Maximum length of the <em>Recently Used</em> and <em>Most Used</em> lists on %Process %Explorer.</td></tr>
 * <tr><td>Process/PerformMallocTrimCalls</td><td>When &gt; 0, call malloc_trim() to release free memory. Only available on Linux; ignored on macOS and Windows.</td></tr>
 * <tr><td>System/NumberOfProcessors</td><td>Total number of processor cores available. Read-only.</td></tr>
 * <tr><td>TransparencyBrush/Brush</td><td>See the pcl::BackgroundBrush namespace.</td></tr>
 * <tr><td>Workspace/PrimaryScreenCenterX</td><td>Read-only. In physical device pixels.</td></tr>
 * <tr><td>Workspace/PrimaryScreenCenterY</td><td>Read-only. In physical device pixels.</td></tr>
 * <tr><td>Workspace/IconGridSpacing</td><td>Grid distance for aligned icon positions on all workspaces. In logical device pixels.</td></tr>
 * </table>
 *
 * <h4>Global Reals</h4>
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>Application/StartJD</td><td>Starting time of the core application instance as a Julian date. Read-only.</td></tr>
 * <tr><td>Application/UIScalingFactor</td><td>Global interface scaling factor of the core application. In the range [1.0,4.0].</td></tr>
 * <tr><td>ImageWindow/DefaultHorizontalResolution</td><td>In device pixels per resolution unit. See the ImageWindow/DefaultMetricResolution global variable.</td></tr>
 * <tr><td>ImageWindow/DefaultVerticalResolution</td><td>In device pixels per resolution unit. See the ImageWindow/DefaultMetricResolution global variable.</td></tr>
 * <tr><td>ImageWindow/PinchSensitivity</td><td>For touch events. In device pixels.</td></tr>
 * <tr><td>MainWindow/ActiveWindowOpacity</td><td>Window opacity in the [0,1] range.</td></tr>
 * <tr><td>MainWindow/AutoHideWindowOpacity</td><td>Window opacity in the [0,1] range.</td></tr>
 * <tr><td>MainWindow/DropShadowBlurRadius</td><td>Radius in pixels of the Gaussian filter applied to drop shadow decorations.</td></tr>
 * <tr><td>MainWindow/InactiveChildWindowOpacity</td><td>Window opacity in the [0,1] range.</td></tr>
 * <tr><td>MainWindow/InactiveWindowOpacity</td><td>Window opacity in the [0,1] range.</td></tr>
 * <tr><td>MainWindow/MovingChildWindowOpacity</td><td>Window opacity in the [0,1] range.</td></tr>
 * <tr><td>MainWindow/MovingWindowOpacity</td><td>Window opacity in the [0,1] range.</td></tr>
 * </table>
 *
 * <h4>Global Colors</h4>
 *
 * Since PixInsight core version 1.8.0, most UI colors and fonts are defined in
 * cascading style sheet files loaded automatically on startup. Only the
 * following four color variables remain because they are purely functional
 * (i.e., not related to the appearance of GUI controls).
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ColorManagement/GamutWarningColor</td><td>Color used for signaling out-of-gamut pixels in color proofing image renditions.</td></tr>
 * <tr><td>MainWindow/DropShadowColor</td><td>The color of drop shadow decorations.</td></tr>
 * <tr><td>TransparencyBrush/BackgroundColor</td><td>Background brush color for renditions of translucent image pixels.</td></tr>
 * <tr><td>TransparencyBrush/DefaultColor</td><td>Opaque color for renditions of translucent image pixels.</td></tr>
 * <tr><td>TransparencyBrush/ForegroundColor</td><td>Foreground brush color for renditions of translucent image pixels.</td></tr>
 * </table>
 *
 * <h4>Global Fonts</h4>
 *
 * Currently there are no global font variables. Since PixInsight core version
 * 1.8.0, all UI fonts are defined through cascading style sheet files loaded
 * automatically on startup.
 *
 * <h4>Global Strings</h4>
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 *
 * <tr><td>Application/AppDirectory</td><td>Full path to the distribution's core application bundle. Read-only. macOS only.</td></tr>
 * <tr><td>Application/AsteroidEphemeridesFile</td><td>File name or path of the distribution's asteroid ephemerides file (XEPH format).</td></tr>
 * <tr><td>Application/AsteroidEphemeridesFilePath</td><td>Full path to the current asteroid ephemerides file (XEPH format). Read-only.</td></tr>
 * <tr><td>Application/BaseDirectory</td><td>Base directory of the core distribution. Read-only.</td></tr>
 * <tr><td>Application/BinDirectory</td><td>Full path to the distribution's bin directory. Read-only.</td></tr>
 * <tr><td>Application/CIP_ITRSDataFile</td><td>File name or path of the distribution's data file of CIP coordinates in the ITRS (plain text).</td></tr>
 * <tr><td>Application/CIP_ITRSDataFilePath</td><td>Full path to the current data file of CIP coordinates in the ITRS (plain text). Read-only.</td></tr>
 * <tr><td>Application/ColorDirectory</td><td>Full path to the distribution's color directory. Read-only.</td></tr>
 * <tr><td>Application/ConfigDirectory</td><td>Full path to the application's configuration directory. Platform-dependent. Read-only.</td></tr>
 * <tr><td>Application/CoreDirectory</td><td>Full directory of the core executable file. Read-only.</td></tr>
 * <tr><td>Application/CoreFilePath</td><td>Full file path of the core executable. Read-only.</td></tr>
 * <tr><td>Application/DeltaATDataFile</td><td>File name or path of the distribution's Delta AT (TAI-UTC) data file (plain text).</td></tr>
 * <tr><td>Application/DeltaATDataFilePath</td><td>Full path to the current Delta AT (TAI-UTC) data file (plain text). Read-only.</td></tr>
 * <tr><td>Application/DeltaTDataFile</td><td>File name or path of the distribution's Delta T (TT-UT1) data file (plain text).</td></tr>
 * <tr><td>Application/DeltaTDataFilePath</td><td>Full path to the current Delta T (TT-UT1) data file (plain text). Read-only.</td></tr>
 * <tr><td>Application/DocDirectory</td><td>Full path to the distribution's doc directory. Read-only.</td></tr>
 * <tr><td>Application/EtcDirectory</td><td>Full path to the distribution's etc directory. Read-only.</td></tr>
 * <tr><td>Application/FiltersDatabaseFile</td><td>File name or path of the distribution's spectrophotometric filter definitions database file (XML format).</td></tr>
 * <tr><td>Application/FiltersDatabaseFilePath</td><td>Full file path to the current spectrophotometric filter definitions database file (XML format). Read-only.</td></tr>
 * <tr><td>Application/FundamentalEphemeridesFile</td><td>File name or path of the distribution's fundamental ephemerides file (XEPH format).</td></tr>
 * <tr><td>Application/FundamentalEphemeridesFilePath</td><td>Full path to the current fundamental ephemerides file (XEPH format). Read-only.</td></tr>
 * <tr><td>Application/HighResFont</td><td>%Font family for automatic style sheet replacement on high-dpi displays.</td></tr>
 * <tr><td>Application/HighResMonoFont</td><td>Monospaced font family for automatic style sheet replacement on high-dpi displays.</td></tr>
 * <tr><td>Application/IncludeDirectory</td><td>Full path to the distribution's include directory. Read-only.</td></tr>
 * <tr><td>Application/LibDirectory</td><td>Full path to the distribution's lib directory. Read-only.</td></tr>
 * <tr><td>Application/LibraryDirectory</td><td>Full path to the distribution's library directory. Read-only.</td></tr>
 * <tr><td>Application/LowResFont</td><td>%Font family for automatic style sheet replacement on low-dpi displays.</td></tr>
 * <tr><td>Application/LowResMonoFont</td><td>Monospaced font family for automatic style sheet replacement on low-dpi displays.</td></tr>
 * <tr><td>Application/NutationModelFile</td><td>File name or path of the distribution's nutation model file (XEPH format).</td></tr>
 * <tr><td>Application/NutationModelFilePath</td><td>Full path to the current nutation model file (XEPH format). Read-only.</td></tr>
 * <tr><td>Application/ResourceFile01</td><td>Core resource file #1.</td></tr>
 * <tr><td>Application/ResourceFile02</td><td>Core resource file #2.</td></tr>
 * <tr><td>Application/ResourceFile03</td><td>Core resource file #3.</td></tr>
 * <tr><td>Application/ResourceFile04</td><td>Core resource file #4.</td></tr>
 * <tr><td>Application/ResourceFile05</td><td>Core resource file #5.</td></tr>
 * <tr><td>Application/ResourceFile06</td><td>Core resource file #6.</td></tr>
 * <tr><td>Application/ResourceFile07</td><td>Core resource file #7.</td></tr>
 * <tr><td>Application/ResourceFile08</td><td>Core resource file #8.</td></tr>
 * <tr><td>Application/ResourceFile09</td><td>Core resource file #9.</td></tr>
 * <tr><td>Application/ResourceFile10</td><td>Core resource file #10.</td></tr>
 * <tr><td>Application/RscDirectory</td><td>Full path to the distribution's rsc directory. Read-only.</td></tr>
 * <tr><td>Application/ShortTermAsteroidEphemeridesFile</td><td>File name or path of the distribution's asteroid ephemerides file (XEPH format) - short-term version (reduced time span).</td></tr>
 * <tr><td>Application/ShortTermAsteroidEphemeridesFilePath</td><td>Full path to the current asteroid ephemerides file (XEPH format) - short-term version (reduced time span). Read-only.</td></tr>
 * <tr><td>Application/ShortTermFundamentalEphemeridesFile</td><td>File name or path of the distribution's fundamental ephemerides file (XEPH format) - short-term version (reduced time span).</td></tr>
 * <tr><td>Application/ShortTermFundamentalEphemeridesFilePath</td><td>Full path to the current fundamental ephemerides file (XEPH format) - short-term version (reduced time span). Read-only.</td></tr>
 * <tr><td>Application/ShortTermNutationModelFile</td><td>File name or path of the distribution's nutation model file (XEPH format) - short-term version (reduced time span).</td></tr>
 * <tr><td>Application/ShortTermNutationModelFilePath</td><td>Full path to the current nutation model file (XEPH format) - short-term version (reduced time span). Read-only.</td></tr>
 * <tr><td>Application/SrcDirectory</td><td>Full path to the distribution's src directory. Read-only.</td></tr>
 * <tr><td>Application/StartTime</td><td>Starting time of the core application in ISO 8601 extended format. Read-only.</td></tr>
 * <tr><td>Application/StyleSheetFile</td><td>Main core stile sheet file.</td></tr>
 * <tr><td>Application/WhiteReferencesDatabaseFile</td><td>File name or path of the distribution's spectrophotometric white reference definitions database file (XML format).</td></tr>
 * <tr><td>Application/WhiteReferencesDatabaseFilePath</td><td>Full file path to the current spectrophotometric white reference definitions database file (XML format). Read-only.</td></tr>
 * <tr><td>ColorManagement/DefaultGrayscaleProfilePath</td><td>Full path to the default ICC color profile for grayscale monochrome images.</td></tr>
 * <tr><td>ColorManagement/DefaultRGBProfilePath</td><td>Full path to the default ICC color profile for RGB color images.</td></tr>
 * <tr><td>ColorManagement/MonitorProfilePath</td><td>Full path to the ICC color profile associated with the primary monitor. Read-only.</td></tr>
 * <tr><td>ColorManagement/ProofingProfilePath</td><td>Full path to the ICC color profile for the color proofing target device.</td></tr>
 * <tr><td>ColorManagement/UpdateMonitorProfile</td><td>Full path to the scheduled new ICC color profile for the primary monitor. Write-only. See the documentation for ColorManagementSetup.</td></tr>
 * <tr><td>FileFormat/ReadFilters</td><td>A list of file filters for file formats capable of image read operations, suitable for file dialogs. Read-only. Updated dynamically on-demand.</td></tr>
 * <tr><td>FileFormat/WriteFilters</td><td>A list of file filters for file formats capable of image write operations, suitable for file dialogs. Read-only. Updated dynamically on-demand.</td></tr>
 * <tr><td>ImageContainerIcon/Prefix</td><td>Prefix for automatically generated image container identifiers.</td></tr>
 * <tr><td>ImageWindow/ClonePostfix</td><td>Postfix appended to image duplicates</td></tr>
 * <tr><td>ImageWindow/DefaultFileExtension</td><td>The default file suffix used to save newly created images.</td></tr>
 * <tr><td>ImageWindow/DownloadsDirectory</td><td>Full path to the core downloads directory.</td></tr>
 * <tr><td>ImageWindow/NewImageCaption</td><td>Window title token for signaling newly created images.</td></tr>
 * <tr><td>ImageWindow/Prefix</td><td>Prefix used for automatically generated image identifiers.</td></tr>
 * <tr><td>ImageWindow/ProxyURL</td><td>The proxy that will be used for core network operations.</td></tr>
 * <tr><td>MainWindow/WallpaperFile01</td><td>Core workspace wallpaper file #1.</td></tr>
 * <tr><td>MainWindow/WallpaperFile02</td><td>Core workspace wallpaper file #2.</td></tr>
 * <tr><td>MainWindow/WallpaperFile03</td><td>Core workspace wallpaper file #3.</td></tr>
 * <tr><td>MainWindow/WallpaperFile04</td><td>Core workspace wallpaper file #4.</td></tr>
 * <tr><td>MainWindow/WallpaperFile05</td><td>Core workspace wallpaper file #5.</td></tr>
 * <tr><td>MainWindow/WallpaperFile06</td><td>Core workspace wallpaper file #6.</td></tr>
 * <tr><td>MainWindow/WallpaperFile07</td><td>Core workspace wallpaper file #7.</td></tr>
 * <tr><td>MainWindow/WallpaperFile08</td><td>Core workspace wallpaper file #8.</td></tr>
 * <tr><td>MainWindow/WallpaperFile09</td><td>Core workspace wallpaper file #9.</td></tr>
 * <tr><td>MainWindow/WallpaperFile10</td><td>Core workspace wallpaper file #10.</td></tr>
 * <tr><td>Preview/Prefix</td><td>Prefix used for automatically generated preview identifiers.</td></tr>
 * <tr><td>ProcessIcon/Prefix</td><td>Prefix used for automatically generated process icon identifiers.</td></tr>
 * <tr><td>View/BrokenLinkText</td><td>Text fragment used to signal broken image and/or process relations, such as missing masks.</td></tr>
 * <tr><td>ViewList/NoPreviewSelectedText</td><td>Text fragment used when there are no previews selected.</td></tr>
 * <tr><td>ViewList/NoPreviewsAvailableText</td><td>Text fragment used when there are no previews available.</td></tr>
 * <tr><td>ViewList/NoViewSelectedText</td><td>Text fragment used when there are no views selected.</td></tr>
 * <tr><td>ViewList/NoViewsAvailableText</td><td>Text fragment used when there are no views available.</td></tr>
 * <tr><td>Workspace/Prefix</td><td>Prefix used for automatically generated workspace identifiers.</td></tr>
 * </table>
 */
class PCL_CLASS PixInsightSettings
{
public:

   /*!
    * Represents the data type of a global variable.
    */
   using variable_type = GlobalVariableType::value_type;

   /*!
    * Default constructor. This constructor is disabled because
    * %PixInsightSettings is not an instantiable class.
    */
   PixInsightSettings() = delete;

   /*!
    * Copy constructor. This constructor is disabled because
    * %PixInsightSettings is not an instantiable class.
    */
   PixInsightSettings( const PixInsightSettings& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because
    * %PixInsightSettings is not an instantiable class.
    */
   PixInsightSettings& operator =( const PixInsightSettings& ) = delete;

   /*!
    * Virtual destructor. This destructor is disabled because
    * %PixInsightSettings is not an instantiable class.
    */
   virtual ~PixInsightSettings() = delete;

   /*!
    * Retrieves the type of a global variable \a globalId.
    */
   static variable_type GlobalVariableType( const IsoString& globalId );

   /*!
    * Returns true iff the specified global variable \a globalId is defined in
    * the current PixInsight platform.
    */
   static bool IsGlobalVariableDefined( const IsoString& globalId )
   {
      return GlobalVariableType( globalId ) != GlobalVariableType::Undefined;
   }

   /*!
    * Returns the value of a global boolean variable.
    */
   static bool GlobalFlag( const IsoString& globalId );

   /*!
    * Returns the value of a global signed integer variable.
    */
   static int GlobalInteger( const IsoString& globalId );

   /*!
    * Returns the value of a global unsigned integer variable.
    */
   static unsigned GlobalUnsigned( const IsoString& globalId );

   /*!
    * Returns the value of a global floating point real variable.
    */
   static double GlobalReal( const IsoString& globalId );

   /*!
    * Returns the value of a global RGBA color variable.
    */
   static RGBA GlobalColor( const IsoString& globalId );

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   /*!
    * Returns the value of a global font face variable.
    */
   static Font GlobalFont( const IsoString& globalId );
#endif

   /*!
    * Returns the value of a global string variable.
    */
   static String GlobalString( const IsoString& globalId );

   /*!
    * Begins a global variable update operation.
    *
    * \sa EndUpdate(), CalcelUpdate()
    */
   static void BeginUpdate();

   /*!
    * Terminates a global variable update operation.
    *
    * \sa BeginUpdate(), CancelUpdate()
    */
   static void EndUpdate();

   /*!
    * Aborts an ongoing global variable update operation.
    *
    * Use this function if you catch an exception during the sequence of
    * SetGlobalXXX() calls. After cancelling an update operation, you
    * shouldn't call EndUpdate().
    *
    * \sa BeginUpdate(), EndUpdate()
    */
   static void CancelUpdate();

   /*!
    * Sets the value of a global boolean variable.
    */
   static void SetGlobalFlag( const IsoString& globalId, bool );

   /*!
    * Sets the value of a global signed integer variable.
    */
   static void SetGlobalInteger( const IsoString& globalId, int );

   /*!
    * Sets the value of a global unsigned integer variable.
    */
   static void SetGlobalUnsigned( const IsoString& globalId, unsigned );

   /*!
    * Sets the value of a global floating point real variable.
    */
   static void SetGlobalReal( const IsoString& globalId, double );

   /*!
    * Sets the value of a global RGBA color variable.
    */
   static void SetGlobalColor( const IsoString& globalId, RGBA );

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   /*!
    * Sets the value of a global font face variable.
    */
   static void SetGlobalFont( const IsoString& globalId, const Font& );
#endif

   /*!
    * Sets the value of a global string variable.
    */
   static void SetGlobalString( const IsoString& globalId, const String& );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_GlobalSettings_h

// ----------------------------------------------------------------------------
// EOF pcl/GlobalSettings.h - Released 2024-12-28T16:53:48Z
