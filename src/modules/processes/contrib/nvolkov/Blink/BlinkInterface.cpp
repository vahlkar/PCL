//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// BlinkInterface.cpp - Released 2021-04-09T19:41:49Z
// ----------------------------------------------------------------------------
// This file is part of the standard Blink PixInsight module.
//
// Copyright (c) 2011-2020 Nikolay Volkov
// Copyright (c) 2003-2020 Pleiades Astrophoto S.L.
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

#include "BlinkInterface.h"
#include "BlinkProcess.h"
#include "BlinkStatisticsDialog.h"
#include "BlinkVideoDialog.h"

#include <pcl/AutoPointer.h>
#include <pcl/Console.h>
#include <pcl/DisplayFunction.h>
#include <pcl/ElapsedTime.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/GlobalSettings.h>
#include <pcl/Graphics.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/ProgressBarStatus.h>
#include <pcl/Random.h>
#include <pcl/Settings.h>
#include <pcl/StandardStatus.h>
#include <pcl/Version.h>
#include <pcl/View.h>

#define WHEEL_STEP_ANGLE   PixInsightSettings::GlobalInteger( "ImageWindow/WheelStepAngle" )

namespace pcl
{

// ----------------------------------------------------------------------------

static int PreviewSize = 202;

// ----------------------------------------------------------------------------

/*
 * Blinking speed in seconds
 */
static const float g_delaySecs[] = { 0.0F, 0.01F, 0.02F, 0.05F, 0.1F, 0.2F, 0.3F, 0.5F, 0.75F, 1.0F, 2.0F, 5.0F };

// ----------------------------------------------------------------------------

template <class P>
static bool LoadImage_P( GenericImage<P>& image, const String& filePath )
{
   Console console;
   console.WriteLn( "<end><cbr><br>* Reading file: " + filePath );
   FileFormat format( File::ExtractExtension( filePath ), true/*toRead*/, false/*toWrite*/ );
   FileFormatInstance file( format );

   try
   {
      ImageDescriptionArray images;

      if ( !file.Open( images, filePath ) )
         throw CaughtException();
      if ( images.IsEmpty() )
         throw Error( filePath + ": Empty image file." );
      if ( images.Length() > 1 )
         console.WarningLn( "<end><cbr>** Warning: Ignoring " + String( images.Length()-1 ) + " additional image(s) in input file." );
      if ( !file.SelectImage( 0 ) )
         throw CaughtException();
      if ( !file.ReadImage( image ) )
         throw CaughtException();
      if ( !file.Close() )
         throw CaughtException();

      ProcessInterface::ProcessEvents();

      return true;
   }
   catch ( ... )
   {
      if ( file.IsOpen() )
         file.Close();

      try
      {
         throw;
      }
      ERROR_HANDLER;

      return false;
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// BlinkInterface::FileData Implementation
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BlinkInterface::FileData::FileData( FileFormatInstance& file,
                                    blink_image* image,
                                    const ImageDescription& description,
                                    const String& path,
                                    bool realPixelData )
   : m_filePath( path )
   , m_image( image )
   , m_options( description.options )
   , m_info( description.info )
   , m_isRealPixelData( realPixelData )
{
   m_format = new FileFormat( file.Format() );

   if ( m_format->UsesFormatSpecificData() )
      m_fsData = file.FormatSpecificData();

   if ( m_format->CanStoreKeywords() )
      file.ReadFITSKeywords( m_keywords );

   if ( m_format->CanStoreICCProfiles() )
      file.ReadICCProfile( m_profile );
}

// ----------------------------------------------------------------------------

BlinkInterface::FileData::~FileData()
{
   if ( m_format != nullptr )
   {
      if ( m_fsData != nullptr )
         m_format->DisposeFormatSpecificData( const_cast<void*>( m_fsData ) ), m_fsData = nullptr;
      delete m_format, m_format = nullptr;
   }

   if ( m_image != nullptr )
      delete m_image, m_image = nullptr;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// BlinkInterface::BlinkData Implementation
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool BlinkInterface::BlinkData::Add( const String& filePath )
{
   Console console;
   console.WriteLn( "<end><cbr>" + filePath );
   FileFormat format( File::ExtractExtension( filePath ), true/*toRead*/, false/*toWrite*/ );
   FileFormatInstance file( format );

   try
   {
      ImageDescriptionArray images;

      if ( !file.Open( images, filePath ) )
         throw CaughtException();
      if ( images.IsEmpty() )
         throw Error( filePath + ": Empty image file." );
      if ( images.Length() > 1 )
         console.WarningLn( "<end><cbr>** Warning: " + filePath +
               ": Multiple images not supported; ignoring " + String( images.Length()-1 ) + " additional image(s)." );
      if ( !file.SelectImage( 0 ) )
         throw CaughtException();
      if ( !CheckGeomery( images[0] ) )
         throw Error( filePath + ": Mismatched image geometry." );

      AutoPointer<blink_image> image( new blink_image );

      if ( !file.ReadImage( *image ) )
         throw CaughtException();

      bool realPixelData = image->IsFloatSample() != images[0].options.ieeefpSampleFormat &&
                           image->BitsPerSample() != images[0].options.bitsPerSample;

      m_filesData.Add( new FileData( file, image.Release(), images[0], filePath, realPixelData ) );

      if ( !file.Close() )
         throw CaughtException();

      return true;
   }
   catch ( ... )
   {
      if ( file.IsOpen() )
         file.Close();

      try
      {
         throw;
      }
      ERROR_HANDLER;

      return false;
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::BlinkData::Remove( int row )
{
   const int fileNumber = FileNumberGet( row );

   m_filesData.Destroy( m_filesData.At( fileNumber ) );

   if ( row == m_blinkMaster )
      m_isBlinkMaster = false;
   if ( row < m_blinkMaster )
      m_blinkMaster--;
   if ( row <= m_currentImage && m_currentImage > 0 )
      m_currentImage--;

   // Correcting links from Files_TreeBox to m_filesData
   for ( int i = 0; i < TheBlinkInterface->GUI->Files_TreeBox.NumberOfChildren(); i++ )
   {
      int n = FileNumberGet( i );
      if ( n > fileNumber ) // Correcting links only for fileNumbers great then curent Removed file #
         TheBlinkInterface->GUI->Files_TreeBox.Child( i )->SetText( 1, String( --n ) );
   }
   TheBlinkInterface->UpdateFileNumbers();

   CheckScreen();
}

// ----------------------------------------------------------------------------

void BlinkInterface::BlinkData::Clear()
{
   m_filesData.Destroy();
   m_isBlinkMaster = false;
   m_blinkMaster = 0;
   m_currentImage = 0;

   CheckScreen();
}

// ----------------------------------------------------------------------------

void BlinkInterface::BlinkData::GetStatsForSTF( int fileNumber )
{
   FileData& fd = m_filesData[fileNumber];
   if ( !fd.m_statSTF.IsEmpty() )
      return;

   StandardStatus callback;
   StatusMonitor m;
   m.SetCallback( &callback );
   m.Initialize( "\nBlink: Calculating statistics", fd.m_image->NumberOfNominalChannels() );
   ProcessEvents();

   for( int c = 0; c < fd.m_image->NumberOfNominalChannels(); c++, ++m )
   {
      fd.m_image->SelectChannel( c );
      ImageStatistics S;
      S.EnableRejection();
      S.SetRejectionLimits( 0.0, 1.0 );
      S.DisableSumOfSquares();
      S.DisableBWMV();
      S.DisablePBMV();
      S << *fd.m_image;
      fd.m_statSTF.Add( S );
   }
   fd.m_image->ResetChannelRange();
   fd.m_isSTFStatisticsEqualToReal = fd.m_isRealPixelData;
}

// ----------------------------------------------------------------------------

void BlinkInterface::BlinkData::AutoSTF()
{
   if ( !CheckScreen() )
      return;

   GetStatsForSTF();

   const int fileNumber = FileNumberGet( m_currentImage );
   const stats_list& S = m_filesData[fileNumber].m_statSTF;
   int n = m_filesData[fileNumber].m_image->NumberOfNominalChannels();
   DisplayFunction DF;
   DF.SetLinkedRGB( TheBlinkInterface->GUI->RGBLinked_Button.IsChecked() );
   Vector sigma( n ), center( n );
   for ( int c = 0; c < n; ++c )
   {
      sigma[c] = 1.4826*S[c].MAD();
      center[c] = S[c].Median();
   }
   DF.ComputeAutoStretch( sigma, center );

   m_screen.MainView().SetScreenTransferFunctions( DF.HistogramTransformations() );

   EnableSTF();
}

// ----------------------------------------------------------------------------

void BlinkInterface::AutoHTThread::Run()
{
   INIT_THREAD_MONITOR()

   for ( int i = m_startIndex; i < m_endIndex; ++i )
   {
      m_blinkData.GetStatsForSTF( i );
      FileData& fd = m_blinkData.m_filesData[i];
      int n = fd.m_image->NumberOfNominalChannels();
      DisplayFunction DF;
      DF.SetLinkedRGB( TheBlinkInterface->GUI->RGBLinked_Button.IsChecked() );
      Vector sigma( n ), center( n );
      for ( int c = 0; c < n; ++c )
      {
         sigma[c] = 1.4826*fd.m_statSTF[c].MAD();
         center[c] = fd.m_statSTF[c].Median();
      }
      DF.ComputeAutoStretch( sigma, center );
      DF >> *fd.m_image;

      UPDATE_THREAD_MONITOR( 1 )
   }
}

void BlinkInterface::BlinkData::AutoHT()
{
   ProgressBarStatus status( "Blink" );
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   monitor.Initialize( "Applying automatic display functions...", m_filesData.Length() );

   AbstractImage::ThreadData data( monitor, m_filesData.Length() );
   Array<size_type> L = Thread::OptimalThreadLoads( m_filesData.Length() );
   ReferenceArray<AutoHTThread> threads;
   for ( int i = 0, n = 0; i < int( L.Length() ); n += int( L[i++] ) )
      threads.Add( new AutoHTThread( data, *this, n, n + int( L[i] ) ) );
   try
   {
      AbstractImage::RunThreads( threads, data );
      threads.Destroy();
      DisableSTF();
      UpdateScreen();
   }
   catch ( ... )
   {
      ResetHT();
      try
      {
         throw;
      }
      catch ( ProcessAborted& )
      {
      }
      catch ( ... )
      {
         throw;
      }
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::BlinkData::ResetHT()
{
   Console console;
   console.Show();
   console.NoteLn( "<end><cbr><br><br>* Blink: Reloading data..." );

   ProgressBarStatus status( "Blink" );
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   monitor.Initialize( "Reloading data...", m_filesData.Length() );

   for ( int fileNumber = 0; fileNumber < int( m_filesData.Length() ); ++fileNumber, ++monitor )
      if ( !LoadImage_P( *m_filesData[fileNumber].m_image, m_filesData[fileNumber].m_filePath ) )
         throw CaughtException();

   console.Hide();

   DisableSTF();
   UpdateScreen();
}

// ----------------------------------------------------------------------------

void BlinkInterface::BlinkData::EnableSTF()
{
   if ( !m_screen.IsNull() )
   {
      m_screen.MainView().EnableScreenTransferFunctions();
      Array<View> previews = m_screen.Previews();
      for ( Array<View>::iterator i = previews.Begin(); i != previews.End(); ++i )
         i->EnableScreenTransferFunctions();
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::BlinkData::DisableSTF()
{
   if ( !m_screen.IsNull() )
   {
      m_screen.MainView().DisableScreenTransferFunctions();
      Array<View> previews = m_screen.Previews();
      for ( Array<View>::iterator i = previews.Begin(); i != previews.End(); ++i )
         i->DisableScreenTransferFunctions();
   }
}

// ----------------------------------------------------------------------------

bool BlinkInterface::BlinkData::CheckScreen()
{
   if ( m_screen.IsNull() ) // must check because user can close BlinkScreen by pushing red X on it.
      return false;

   if ( m_filesData.IsEmpty() ) // close Screen if no files
   {
      m_screen.Close();
      m_screen = ImageWindow::Null();
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool BlinkInterface::BlinkData::CheckGeomery( const ImageDescription& description )
{
   const Rect imageBounds( description.info.width, description.info.height );

   if ( m_filesData.IsEmpty() )
   {
      m_statRect = m_screenRect = imageBounds;
      m_info = description.info;
      m_options = description.options;
   }
   else if ( imageBounds != m_screenRect )
   {
      Console().CriticalLn( String().Format( "<end><cbr>*** Error: Blink: Mismatched image geometry: Got %dx%d pixels, expected %dx%d.",
                                             imageBounds.Width(), imageBounds.Height(),
                                             m_screenRect.Width(), m_screenRect.Height() ) );
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

void BlinkInterface::BlinkData::Next()
{
   const int wasCurrent = m_currentImage;
   while ( true )
   {
      m_currentImage++;
      if ( m_currentImage >= int( m_filesData.Length() ) )
         m_currentImage = 0;
      if ( m_currentImage == wasCurrent ) // one round passed
         return;  // no new images
      if ( m_isBlinkMaster && m_currentImage == m_blinkMaster )
         continue; // skip blinkMaster
      if ( TheBlinkInterface->GUI->Files_TreeBox.Child( m_currentImage )->IsChecked() ) // skip unchecked
         break;
   }
   Update();
}

// ----------------------------------------------------------------------------

void BlinkInterface::BlinkData::Prev()
{
   const int wasCurrent = m_currentImage;
   while ( true )
   {
      if ( m_currentImage == 0 )
         m_currentImage = int( m_filesData.Length() );
      m_currentImage--;
      if ( m_currentImage == wasCurrent ) // one round passed
         break; // no new images
      if ( TheBlinkInterface->GUI->Files_TreeBox.Child( m_currentImage )->IsChecked() )
         break;
   }
   Update();
}

// ----------------------------------------------------------------------------

void BlinkInterface::BlinkData::UpdateScreen( int fileNumber )
{
   if ( m_screen.IsNull() )
   {
      if ( m_filesData.IsEmpty() )
         return;

      m_screen = ImageWindow( m_info.width, m_info.height, m_info.numberOfChannels,
                              m_options.bitsPerSample, m_options.ieeefpSampleFormat,
                              IsColor(), true, "BlinkScreen" );

      if ( TheBlinkInterface->GUI->AutoSTF_Button.IsChecked() )
         AutoSTF();

      m_screen.BringToFront();
      m_screen.Show();
   }

   FileData& fd = m_filesData[fileNumber];
   View view = m_screen.CurrentView();
   if( view.IsMainView() )
      view.Image().CopyImage( *fd.m_image );
   else
   {
      fd.m_image->SelectRectangle( m_screen.PreviewRect( view.Id() ) );
      view.Image().CopyImage( *fd.m_image );
      fd.m_image->ResetSelection();
   }
   m_screen.Regenerate();
}

// ----------------------------------------------------------------------------

void BlinkInterface::BlinkData::Update( int row )
{
   UpdateScreen( row );
   TheBlinkInterface->GUI->Files_TreeBox.SetCurrentNode( TheBlinkInterface->GUI->Files_TreeBox.Child( row ) );
}

// ----------------------------------------------------------------------------

void BlinkInterface::BlinkData::ShowNextImage()
{
   if ( m_isBlinkMaster )
   {
      static bool showBlinkMaster = false; // trigger
      showBlinkMaster = !showBlinkMaster;
      if ( showBlinkMaster )
      {
         //while PCL don't have events on TreeBox sorting, we must check blinkMaster row position every time
         if ( TheBlinkInterface->GUI->Files_TreeBox.Child( m_blinkMaster )->Icon( 0 ).IsNull() )
         {
            //row of blinkMaster chenged >> find row with blinkMaster
            for ( int i = 0; i < TheBlinkInterface->GUI->Files_TreeBox.NumberOfChildren(); ++i )
               if ( !TheBlinkInterface->GUI->Files_TreeBox.Child( i )->Icon( 0 ).IsNull() )
               {
                  m_blinkMaster = i;
                  break;
               }
         }
         Update( m_blinkMaster );
      }
      else
         Next();
   }
   else
      Next();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// BlinkInterface Implementation
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BlinkInterface* TheBlinkInterface = nullptr;

// ----------------------------------------------------------------------------

BlinkInterface::BlinkInterface()
{
   TheBlinkInterface = this;

#ifdef __PCL_WINDOWS
   m_program = "ffmpeg.exe";
#else
   m_program = "ffmpeg";
#endif
   m_outputFileName = Id();
   // Default video encoder arguments
   m_arguments= "-y -r 25 -i " + m_outputFileName + "%05d" + m_frameExtension + ' ' + m_outputFileName + ".avi";
}

// ----------------------------------------------------------------------------

BlinkInterface::~BlinkInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString BlinkInterface::Id() const
{
   return "Blink";
}

// ----------------------------------------------------------------------------

MetaProcess* BlinkInterface::Process() const
{
   return TheBlinkProcess;
}

// ----------------------------------------------------------------------------

String BlinkInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/Blink.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures BlinkInterface::Features() const
{
   return InterfaceFeature::None;
}

// ----------------------------------------------------------------------------

bool BlinkInterface::IsInstanceGenerator() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool BlinkInterface::CanImportInstances() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool BlinkInterface::Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Blink" );
      PreviewSize = GUI->Preview_ScrollBox.Width(); // already scaled to logical units
   }

   Init();

   dynamic = false;
   return true;
}

// ----------------------------------------------------------------------------

void BlinkInterface::SaveSettings() const
{
   Settings::Write( SettingsKey() + "_SortChannels",      m_sortChannels );
   Settings::Write( SettingsKey() + "_CropMode",          m_cropMode );
   Settings::Write( SettingsKey() + "_Digits0_1",         m_digits0_1 );
   Settings::Write( SettingsKey() + "_Digits0_65535",     m_digits0_65535 );
   Settings::Write( SettingsKey() + "_Range0_65535",      m_range0_65535 );
   Settings::Write( SettingsKey() + "_WriteStatsFile",    m_writeStatsFile );

   // Video generation parameters are not saved automatically. They are saved
   // manually from BlinkVideoDialog.
   //SaveVideoSettings();
}

// ----------------------------------------------------------------------------

void BlinkInterface::SaveVideoSettings() const
{
   Settings::Write( SettingsKey() + "_Program",           m_program );
   Settings::Write( SettingsKey() + "_Arguments",         m_arguments );
   Settings::Write( SettingsKey() + "_FrameOutputDir",    m_frameOutputDir );
   Settings::Write( SettingsKey() + "_FrameExtension",    m_frameExtension );
   Settings::Write( SettingsKey() + "_DeleteVideoFrames", m_deleteVideoFrames );
}

// ----------------------------------------------------------------------------

void BlinkInterface::LoadSettings()
{
   Settings::Read( SettingsKey() + "_SortChannels",       m_sortChannels );
   Settings::Read( SettingsKey() + "_CropMode",           m_cropMode );
   Settings::Read( SettingsKey() + "_Digits0_1",          m_digits0_1 );
   Settings::Read( SettingsKey() + "_Digits0_65535",      m_digits0_65535 );
   Settings::Read( SettingsKey() + "_Range0_65535",       m_range0_65535 );
   Settings::Read( SettingsKey() + "_ReadStatsFile",      m_writeStatsFile );

   LoadVideoSettings();
}

// ----------------------------------------------------------------------------

void BlinkInterface::LoadVideoSettings()
{
   Settings::Read( SettingsKey() + "_Program",            m_program );
   Settings::Read( SettingsKey() + "_Arguments",          m_arguments );
   Settings::Read( SettingsKey() + "_FrameOutputDir",     m_frameOutputDir );
   Settings::Read( SettingsKey() + "_FrameExtension",     m_frameExtension );
   Settings::Read( SettingsKey() + "_DeleteVideoFrames",  m_deleteVideoFrames );
}

// ----------------------------------------------------------------------------

bool BlinkInterface::WantsImageNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void BlinkInterface::ImageCreated( const View& view )
{
   /*
    * If a new preview has been created on our bliking screen window and we are
    * blinking, get possession of it by locking it for write operations.
    */
   if ( m_isRunning )
      if ( view.IsPreview() )
         if ( view.Window() == m_blink.m_screen )
            if ( !view.CanWrite() ) // other task owns a preview on our blinking screen window !? - stop blinking
               Stop();
            else
               view.LockForWrite();
}

// ----------------------------------------------------------------------------

void BlinkInterface::ImageDeleted( const View& view )
{
   /*
    * Our blinking screen window has been destroyed.
    */
   if ( !m_blink.m_screen.IsNull() )
      if ( view.Window() == m_blink.m_screen )
         m_blink.m_screen = ImageWindow::Null();
}

// ----------------------------------------------------------------------------

void BlinkInterface::Init()
{
   // Disable all controls during heavy interface updates.
   // ### N.B. Simply calling Disable()/Enable() here (the obvious way of doing
   // this) seems to trigger several Qt bugs as of Qt 5.15.0. Doing the same
   // for both child controls is 100% stable.
   GUI->CentralPanel_Control.Disable();
   GUI->RightPanel_Control.Disable();

   bool noFiles = m_blink.m_filesData.IsEmpty(); // noFiles = disabled = no files

   GUI->AutoHT_Button.Disable( noFiles );
   GUI->AutoSTF_Button.Disable( noFiles );
   GUI->FileClose_Button.Disable( noFiles );
   GUI->FileCloseAll_Button.Disable( noFiles );
   GUI->FileCopyTo_Button.Disable( noFiles );
   GUI->FileMoveTo_Button.Disable( noFiles );
   GUI->FileCropTo_Button.Disable( noFiles );
   GUI->Statistics_button.Disable( noFiles );
   GUI->CropToVideo_button.Disable( noFiles );

   if ( noFiles )             // file == 0 -> init default GUI
   {
      GUI->AutoHT_Button.Uncheck();
      GUI->AutoSTF_Button.SetChecked();
      GUI->RGBLinked_Button.Hide();
      GUI->RGBLinked_Button.SetChecked();
      GUI->Preview_ScrollBox.Update();
   }
   else
   {
      if ( m_blink.IsColor() )
         GUI->RGBLinked_Button.Show();
      m_blink.Update();
      GUI->UpdatePreview_Timer.Start();
   }

   bool oneOrNone = m_blink.m_filesData.Length() < 2;  // disabled = zero or one file only
   GUI->PreviousImage_Button.Disable( oneOrNone );
   GUI->Play_Button.Disable( oneOrNone );
   GUI->NextImage_Button.Disable( oneOrNone );
   GUI->BlinkingDelay_ComboBox.Disable( oneOrNone );

   GeneratePreview();

   GUI->CentralPanel_Control.Enable();
   GUI->RightPanel_Control.Enable();
}

// ----------------------------------------------------------------------------

void BlinkInterface::TranslucentPlanets()
{
   const int  maxRadius = 30;               // Maximum planet radius
   const int  numberOfPlanets = 50;         // Number of translucent planets
   const int  networkFrequency = 10;        // Frequency of network lines
   const RGBA skyTopColor = 0xFF000000;     // Top background color (solid black by default)
   const RGBA skyBottomColor = 0xFF00008B;  // Bottom background color (dark blue by default)
   const RGBA networkColor = 0xFFFF8000;    // Network color (solid orange by default)
   const RGBA networkBkgColor = 0xFF000000; // Network background color (solid black by default)
   const int  planetTransparency = 0x80;    // Alpha value of all random planet colors

   m_previewBmp = Bitmap( PreviewSize, PreviewSize, BitmapFormat::RGB32 );

   // Begin painting
   Graphics g( m_previewBmp );

   // We want high-quality antialiased graphics
   g.EnableAntialiasing();

   // Fill the background with a linear gradient
   GradientBrush::stop_list stops;
   stops.Add( GradientBrush::Stop( 0.0, skyTopColor ) );
   stops.Add( GradientBrush::Stop( 1.0, skyBottomColor ) );
   g.FillRect( Rect( PreviewSize, PreviewSize ),
               LinearGradientBrush( Rect( PreviewSize, PreviewSize ), stops ) );

   // Draw random circles
   RandomNumberGenerator randomColor( 255 );
   RandomNumberGenerator R;
   for ( int i = 0; i < numberOfPlanets; ++i )
   {
      // Random colors in the range [0,255]
      int red = randomColor();
      int green = randomColor();
      int blue = randomColor();

      // Avoid too dark circles
      if ( red < 24 && green < 24 && blue < 24 ) { --i; continue; }

      // 32-bit RRGGBBAA color values
      RGBA color1 = RGBAColor( red, green, blue, planetTransparency );
      RGBA color2 = RGBAColor( red >> 1, green >> 1, blue >> 1, planetTransparency );

      // Random center and radius
      Point center( PreviewSize * R(), PreviewSize * R() );
      double radius = maxRadius * R();

      // Define working objects
      GradientBrush::stop_list stops;
      stops.Add( GradientBrush::Stop( 0.0, color1 ) );
      stops.Add( GradientBrush::Stop( 1.0, color2 ) );

      g.SetPen( color2 );
      g.SetBrush( RadialGradientBrush( center, radius, center, stops ) );

      // Draw this planet
      g.DrawCircle( center, radius );
   }

   // Erase the network region by drawing a dense network
   g.DisableAntialiasing();
   g.SetPen( networkBkgColor );
   for ( int i = 0; i < PreviewSize; ++i )
      g.DrawLine( i-1, PreviewSize, -1, i+1 );

   // Generate the network
   g.EnableAntialiasing();
   g.SetPen( networkColor );
   for ( int i = 0; i < PreviewSize; i += networkFrequency )
      g.DrawLine( i, PreviewSize-1, 0, i );
   g.DrawLine( PreviewSize-1, PreviewSize-1, 0, PreviewSize-1 );
}

// ----------------------------------------------------------------------------

void BlinkInterface::Image2Preview()
{
   blink_image image( *m_blink.m_filesData[0].m_image );

   if ( GUI->AutoSTF_Button.IsChecked() )
   {
      m_blink.GetStatsForSTF( 0 );
      const stats_list& S = m_blink.m_filesData[0].m_statSTF;
      int n = image.NumberOfNominalChannels();
      DisplayFunction DF;
      DF.SetLinkedRGB( TheBlinkInterface->GUI->RGBLinked_Button.IsChecked() );
      Vector sigma( n ), center( n );
      for ( int c = 0; c < n; ++c )
      {
         sigma[c] = 1.4826*S[c].MAD();
         center[c] = S[c].Median();
      }
      DF.ComputeAutoStretch( sigma, center );
      DF >> image;
   }

   int w = (image.Width() > image.Height()) ? PreviewSize : RoundInt( float( PreviewSize )*image.Width()/image.Height() );
   int h = (image.Height() > image.Width()) ? PreviewSize : RoundInt( float( PreviewSize )*image.Height()/image.Width() );
   m_previewBmp = Bitmap::Render( &image ).ScaledToSize( w, h ); // fit big Image to small Preview
}

// ----------------------------------------------------------------------------

void BlinkInterface::GeneratePreview()
{
   if ( m_blink.m_filesData.IsEmpty() )
      TranslucentPlanets();
   else
      Image2Preview();

   GUI->Preview_ScrollBox.SetFixedSize( m_previewBmp.Width(), m_previewBmp.Height() );
}

// ----------------------------------------------------------------------------

void BlinkInterface::UpdateFileNumbers()
{
   GUI->Files_TreeBox.DisableHeaderSorting();
   for ( int i = 0; i < GUI->Files_TreeBox.NumberOfChildren(); i++ )
   {
      RowToStringFileNumber( i );
      GUI->Files_TreeBox.Child( i )->SetAlignment( 1, TextAlign::Right );
      GUI->Files_TreeBox.Child( i )->SetAlignment( 2, TextAlign::Right );
   }
   GUI->Files_TreeBox.AdjustColumnWidthToContents( 2 );
   GUI->Files_TreeBox.EnableHeaderSorting();
}

// ----------------------------------------------------------------------------

void BlinkInterface::AddFiles( const StringList& files )
{
   if ( !files.IsEmpty() )
   {
      size_type N = files.Length();

      Console console;
      console.Show();
      console.NoteLn( "<end><cbr><br><br>* Blink: Loading " + String( N ) + " file(s)" );

      ProgressBarStatus status( "Blink" );
      StatusMonitor monitor;
      monitor.SetCallback( &status );
      monitor.Initialize( "Loading data...", N );

      GUI->Files_TreeBox.DisableUpdates();

      try
      {
         ElapsedTime timer;  // to calculate execution time

         for ( const String& file : files )
         {
            if ( !m_blink.Add( file ) )  // add the file to BlinkData
               continue;                 // skip the file on error

            TreeBox::Node* node = new TreeBox::Node( GUI->Files_TreeBox ); // add new item in Files_TreeBox
            node->Check();                                                 // check new file items
            node->SetText( 0, File::ExtractName( file ) );                 // show only the file name
            node->SetText( 1, String( GUI->Files_TreeBox.NumberOfChildren()-1 ) ); // Store file #

            ProcessEvents();

            ++monitor;
         }

         //GUI->Files_TreeBox.AdjustToContents(); ### don't do this, since it resizes the whole interface!
         GUI->Files_TreeBox.AdjustColumnWidthToContents( 0 );
         GUI->Files_TreeBox.AdjustColumnWidthToContents( 1 );

         UpdateFileNumbers();
         GUI->Files_TreeBox.EnableUpdates();

         console.WriteLn( "<end><cbr><br>Loaded in " + timer.ToString() );
         console.Hide();

         Init();
      }
      catch ( ... )
      {
         GUI->Files_TreeBox.EnableUpdates();
         FileCloseAll();
         throw;
      }
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::FileAdd()
{
   OpenFileDialog d;
   d.LoadImageFilters();
   d.EnableMultipleSelections();
   d.SetCaption( "Blink/FileOpen: Select Image Files" );
   if ( d.Execute() )
      AddFiles( d.FileNames() );
}

// ----------------------------------------------------------------------------

void BlinkInterface::FileCopyTo()
{
   Pause();

   const String dir( SelectDirectory( "Blink/FileCopy: Select Output Directory" ) );
   if ( dir.IsEmpty() )
   {
      Continue();
      return;
   }

   size_type N = GUI->Files_TreeBox.SelectedNodes().Length();

   Console console;
   console.Show();
   console.NoteLn( "<end><cbr><br><br>* Blink/Copy: " + String( N ) + " file(s) selected." );

   ProgressBarStatus status( "Blink" );
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   monitor.Initialize( "Copying files...", N );

   try
   {
      for ( int row = 0; row < GUI->Files_TreeBox.NumberOfChildren(); ++row )
         if ( GUI->Files_TreeBox.Child( row )->IsSelected() )
         {
            const int fileNumber = FileNumberGet( row );
            const String fromFilePath = m_blink.m_filesData[fileNumber].m_filePath;
            const String toFilePath = UniqueFilePath( fromFilePath, dir );
            console.WriteLn( "Copy from " + fromFilePath + " to " + toFilePath );
            ProcessEvents();
            File::CopyFile( toFilePath, fromFilePath );
            ++monitor;
         }

      console.WriteLn( "Done." );
      console.Hide();

      Continue();
   }
   catch ( ProcessAborted& )
   {
   }
   catch ( ... )
   {
      Stop();
      Exception::EnableConsoleOutput();
      try
      {
         throw;
      }
      ERROR_HANDLER
      Exception::DisableConsoleOutput();
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::FileMoveTo()
{
   Pause();

   const String dir( SelectDirectory( "Blink/FileMove: Select Output Directory" ) );
   if ( dir.IsEmpty() )
   {
      Continue();
      return;
   }

   size_type N = GUI->Files_TreeBox.SelectedNodes().Length();

   Console console;
   console.Show();
   console.NoteLn( "<end><cbr><br><br>* Blink/Move: " + String( N ) + " file(s) selected." );

   ProgressBarStatus status( "Blink" );
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   monitor.Initialize( "Moving files...", N );

   try
   {
      for ( int row = 0; row < GUI->Files_TreeBox.NumberOfChildren(); row++ )
         if ( GUI->Files_TreeBox.Child( row )->IsSelected() )
         {
            const int fileNumber = FileNumberGet( row );
            const String fromFilePath = m_blink.m_filesData[fileNumber].m_filePath;
            const String toFilePath = UniqueFilePath( fromFilePath, dir );
            console.WriteLn( "Move from " + fromFilePath + " to " + toFilePath );
            ProcessEvents();
            File::MoveFile( toFilePath, fromFilePath );
            m_blink.m_filesData[fileNumber].m_filePath = toFilePath; // update file path
            ++monitor;
         }

      console.WriteLn( "Done." );
      console.Hide();

      Continue();
   }
   catch ( ProcessAborted& )
   {
   }
   catch ( ... )
   {
      Stop();
      Exception::EnableConsoleOutput();
      try
      {
         throw;
      }
      ERROR_HANDLER
      Exception::DisableConsoleOutput();
   }
}

// ----------------------------------------------------------------------------

Rect BlinkInterface::GetCropRect()
{
   if ( m_blink.m_screen.SelectedPreview().IsNull() )
      return m_blink.m_screen.ViewportToImage( m_blink.m_screen.VisibleViewportRect() );
   else
   {
      const Point p = m_blink.m_screen.PreviewRect( m_blink.m_screen.SelectedPreview().Id() ).LeftTop();
      return m_blink.m_screen.SelectedPreview().Bounds().MovedTo( p );
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::CropToVideo()
{
   Stop();

   if ( m_blink.m_screen.IsNull() )
   {
      pcl::MessageBox( "<p>The BlinkScreen image window has been closed - the cropping region is undefined.</p>",
                        "Blink", StdIcon::Information ).Execute();
      return;
   }

   BlinkVideoDialog dialog( this );
   dialog.Execute();
}

// ----------------------------------------------------------------------------

void BlinkInterface::FileCropTo()
{
   if ( m_blink.m_screen.IsNull() )
   {
      pcl::MessageBox( "<p>The BlinkScreen image window has been closed - the cropping region is undefined.</p>",
                        "Blink",
                        StdIcon::Information ).Execute();
      return;
   }

   Pause();

   Rect r( GetCropRect() );
   if ( r == m_blink.m_screenRect )
   {
      pcl::MessageBox( "<p>The entire image has been selected. "
                        "Either create a partial preview in BlinkScreen, or zoom in the image to define a valid cropping region.</p>",
                        "Blink", StdIcon::Information ).Execute();
      return;
   }

   const String dir( SelectDirectory( "Blink/Crop: Select Output Directory" ) );
   if ( dir.IsEmpty() )
      return;

   size_type N = GUI->Files_TreeBox.SelectedNodes().Length();

   Console console;
   console.Show();
   console.NoteLn( "<end><cbr><br><br>Blink/Crop: " + String( N ) + " file(s) selected." );

   ProgressBarStatus status( "Blink" );
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   monitor.Initialize( "Writing cropped files...", N );

   try
   {
      for ( int row = 0; row < GUI->Files_TreeBox.NumberOfChildren(); ++row )
         if ( GUI->Files_TreeBox.Child( row )->IsSelected() )
         {
            const int fileNumber = FileNumberGet( row );
            FileFormatInstance outputFile( CreateImageFile( fileNumber, "CropTo", dir ) );

            FileData& fd = m_blink.m_filesData[fileNumber];

            if ( fd.m_isRealPixelData )
            {
               fd.m_image->SelectRectangle( r );
               if ( !outputFile.WriteImage( *fd.m_image ) )
                  throw CaughtException();
               fd.m_image->ResetSelection();
            }
            else
            {
               Image tmp;
               if ( !LoadImage_P( tmp, fd.m_filePath ) )
                  throw CaughtException();

               tmp.SelectRectangle( r );
               if ( !outputFile.WriteImage( tmp ) )
                  throw CaughtException();
            }

            if ( !outputFile.Close() )
               throw CaughtException();

            ProcessEvents();

            ++monitor;
         }

      console.WriteLn( "Done." );
      console.Hide();

      Continue();
   }
   catch ( ProcessAborted& )
   {
   }
   catch ( ... )
   {
      Stop();
      Exception::EnableConsoleOutput();
      try
      {
         throw;
      }
      ERROR_HANDLER
      Exception::DisableConsoleOutput();
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::FileCloseSelected()
{
   Pause();

   GUI->Files_TreeBox.DisableUpdates();
   for ( int i = int( m_blink.m_filesData.Length() ); --i >= 0; )
      if ( GUI->Files_TreeBox.Child( i )->IsSelected() )
      {
         m_blink.Remove( i );
         GUI->Files_TreeBox.Remove( i );
      }
   GUI->Files_TreeBox.EnableUpdates();

   if ( m_blink.m_filesData.Length() < 2 )
   {
      Stop();
      Init();
      if ( m_blink.m_filesData.IsEmpty() )
         ResetFilesTreeBox();
   }
   else
   {
      m_blink.Update();
      Continue();
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::FileCloseAll()
{
   Console().NoteLn( "<end><cbr><br><br>* Blink: Releasing memory... " );
   Stop();
   m_blink.Clear();
   ResetFilesTreeBox();
   Init();
}

// ----------------------------------------------------------------------------

void BlinkInterface::FileStatistics()
{
   Pause();
   BlinkStatisticsDialog dialog( this );
   dialog.Execute();
   Continue();
}

// ----------------------------------------------------------------------------

void BlinkInterface::DisableButtonsIfRunning()
{
   GUI->AutoHT_Button.Disable( m_isRunning );
   GUI->AutoSTF_Button.Disable( m_isRunning );
   GUI->RGBLinked_Button.Disable( m_isRunning );
   GUI->FileAdd_Button.Disable( m_isRunning );
   GUI->FileClose_Button.Disable( m_isRunning );
   GUI->FileCloseAll_Button.Disable( m_isRunning );
   GUI->FileCopyTo_Button.Disable( m_isRunning );
   GUI->FileMoveTo_Button.Disable( m_isRunning );
   GUI->FileCropTo_Button.Disable( m_isRunning );
   GUI->Statistics_button.Disable( m_isRunning );
   GUI->CropToVideo_button.Disable( m_isRunning );
}

// ----------------------------------------------------------------------------

void BlinkInterface::Play()
{
   Stop();

   GUI->Play_Button.SetIcon( Bitmap( ScaledResource( ":/icons/pause.png" ) ) );
   GUI->Play_Button.SetToolTip( "Pause Animation" );
   DisableButtonsIfRunning();

   /*
    * Ensure the screen window exists and is visible, in case it was closed.
    */
   m_blink.UpdateScreen();

   /*
    * Lock the blink screen window's main view for writing. This implicitly
    * locks all existing previews.
    */
   m_blink.m_screen.MainView().LockForWrite();

   m_isRunning = true;
   GUI->UpdateAnimation_Timer.Start();
}

// ----------------------------------------------------------------------------

void BlinkInterface::Stop()
{
   GUI->UpdateAnimation_Timer.Stop();

   if ( m_isRunning )
   {
      /*
       * Unlock all locked blink screen views
       */
      if ( m_blink.CheckScreen() ) // this check not necessary in theory, but doesn't hurt either
      {
         Array<View> previews = m_blink.m_screen.Previews();
         for ( Array<View>::iterator i = previews.Begin(); i != previews.End(); ++i )
            if ( !i->CanWrite() )
               i->Unlock();
         if ( !m_blink.m_screen.MainView().CanWrite() )
            m_blink.m_screen.MainView().Unlock();
      }
      m_isRunning = false;
   }

   GUI->Play_Button.SetIcon( Bitmap( ScaledResource( ":/icons/play.png" ) ) );
   GUI->Play_Button.SetToolTip( "Play Animation" );
   DisableButtonsIfRunning();
}

// ----------------------------------------------------------------------------

void BlinkInterface::Pause()
{
   if ( !m_blink.m_filesData.IsEmpty() )
      GUI->UpdatePreview_Timer.Stop();
   if ( m_isRunning )
      GUI->UpdateAnimation_Timer.Stop();
}

// ----------------------------------------------------------------------------

void BlinkInterface::Continue()
{
   if ( !m_blink.m_filesData.IsEmpty() )
      GUI->UpdatePreview_Timer.Start();
   if ( m_isRunning )
      GUI->UpdateAnimation_Timer.Start();
}

// ----------------------------------------------------------------------------
// Event Handlers
// ----------------------------------------------------------------------------

void BlinkInterface::__Brightness_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AutoHT_Button )
   {
      Pause();

      if ( checked )
      {
         if ( GUI->AutoSTF_Button.IsChecked() )
            GUI->AutoSTF_Button.Uncheck();

         m_blink.AutoHT();
      }
      else
         m_blink.ResetHT();

      if ( m_blink.m_screen.IsNull() )
         m_blink.UpdateScreen();

      GeneratePreview();

      Continue();
   }
   else if ( sender == GUI->AutoSTF_Button )
   {
      Pause();

      if ( checked )
      {
         if ( GUI->AutoHT_Button.IsChecked() )
         {
            GUI->AutoHT_Button.Uncheck();
            m_blink.ResetHT();
         }

         m_blink.AutoSTF();
      }
      else
         m_blink.DisableSTF();

      if ( m_blink.m_screen.IsNull() )
         m_blink.UpdateScreen();

      GeneratePreview();

      Continue();
   }
   else if ( sender == GUI->RGBLinked_Button )
   {
      Pause();

      if ( GUI->AutoSTF_Button.IsChecked() )
      {
         m_blink.AutoSTF();
         GeneratePreview();
      }
      else if ( GUI->AutoHT_Button.IsChecked() )
      {
         m_blink.ResetHT();
         m_blink.AutoHT();
         GeneratePreview();
      }

      Continue();
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::__Files_NodeSelectionUpdated( TreeBox& sender )
{
   m_blink.m_currentImage = sender.ChildIndex( sender.CurrentNode() );
   m_blink.UpdateScreen();
}

// ----------------------------------------------------------------------------

void BlinkInterface::__Files_NodeUpdated( TreeBox& sender, TreeBox::Node& node, int col  )
{
   for ( size_type i = 0; i < sender.SelectedNodes().Length(); i++ ) // Invert CheckMark
   {
      if ( sender.SelectedNodes()[i]->IsSameObject( node ) )
         continue;   // skip curent clicked node, because it will be inverted automaticaly
      sender.SelectedNodes()[i]->Uncheck( sender.SelectedNodes()[i]->IsChecked() );
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::__Files_MouseWheel( Control& sender, const Point& pos, int delta, unsigned buttons, unsigned modifiers )
{
   if ( delta > 0 )
      m_blink.Prev();
   else
      m_blink.Next();
}

// ----------------------------------------------------------------------------

void BlinkInterface::__Files_NodeDoubleClicked( TreeBox& sender, TreeBox::Node& node, int col  )
{
   if ( m_blink.m_isBlinkMaster )
   {
      sender.Child( m_blink.m_blinkMaster )->SetIcon( 0, Bitmap() );
      if ( m_blink.m_blinkMaster == sender.ChildIndex( &node ) )
      {
         m_blink.m_isBlinkMaster = false;
         return;
      }
   }

   m_blink.m_isBlinkMaster = true;
   m_blink.m_blinkMaster = sender.ChildIndex( &node );
   node.SetIcon( 0, Bitmap( ScaledResource( ":/icons/repeat.png" ) ) );
}

// ----------------------------------------------------------------------------

void BlinkInterface::__FileButton_Click( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->FileAdd_Button )
      FileAdd();
   else if ( sender == GUI->FileClose_Button )
      FileCloseSelected();
   else if ( sender == GUI->FileCloseAll_Button )
      FileCloseAll();
   else if ( sender == GUI->FileCopyTo_Button )
      FileCopyTo();
   else if ( sender == GUI->FileMoveTo_Button )
      FileMoveTo();
   else if ( sender == GUI->FileCropTo_Button )
      FileCropTo();
   else if ( sender == GUI->Statistics_button )
      FileStatistics();
   else if ( sender == GUI->CropToVideo_button )
      CropToVideo();
}

// ----------------------------------------------------------------------------

void BlinkInterface::__Delay_ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   GUI->UpdateAnimation_Timer.SetInterval( g_delaySecs[itemIndex] );
   if ( m_isRunning )
      GUI->UpdateAnimation_Timer.Start();
}

// ----------------------------------------------------------------------------

void BlinkInterface::__ActionButton_Click( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->Play_Button && !m_isRunning )
   {
      Play();
      return;
   }

   if ( m_isRunning )
      Stop();

   if ( sender == GUI->NextImage_Button )
      m_blink.Next();
   else if ( sender == GUI->PreviousImage_Button )
      m_blink.Prev();
}

// ----------------------------------------------------------------------------

void BlinkInterface::__ScrollControl_Paint( Control& sender, const Rect& r )
{
   if ( m_previewBmp.IsNull() )
      return;

   Graphics g( sender );
   g.DisableAntialiasing();

   g.DrawBitmapRect( r.LeftTop(), m_previewBmp, r );

   if ( m_blink.m_screen.IsNull() || m_blink.m_filesData.IsEmpty() )
      return;

   DRect destRect = m_blink.m_screen.ViewportToImage( m_blink.m_screen.VisibleViewportRect() );
   if ( m_blink.m_screen.CurrentView().IsPreview() )
      destRect += m_blink.m_screen.PreviewRect( m_blink.m_screen.CurrentView().Id() ).LeftTop();

   if ( m_blink.m_screen.SelectedPreview().IsNull() )
      g.SetPen( 0xFF00FF00 ); // green
   else
      g.SetPen( 0xFFFFFFFF ); // white

   double k = (m_blink.m_info.width > m_blink.m_info.height) ?
                              double( sender.Width() )/m_blink.m_info.width :
                              double( sender.Height() )/m_blink.m_info.height;
   g.DrawRect( (k * destRect).RoundedToInt() );

   if ( m_blink.m_screen.HasPreviews() )
   {
      Array<View> previews = m_blink.m_screen.Previews();
      for ( size_type i = 0; i < previews.Length(); i++ )
      {
         if ( m_blink.m_screen.SelectedPreview() == previews[i] )
            g.SetPen( 0xFF00FF00 ); // green
         else
            g.SetPen( 0xFFFFFFFF ); // white

         Point p = m_blink.m_screen.PreviewRect( previews[i].Id() ).LeftTop();
         g.DrawRect( (k * DRect( previews[i].Bounds().MovedTo( p ) )).RoundedToInt() );
      }
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::__ScrollControl_MouseWheel( Control& sender, const Point& pos, int delta, unsigned buttons, unsigned modifiers )
{
   if ( m_blink.m_screen.IsNull() || m_blink.m_filesData.IsEmpty() )
      return;

   m_wheelSteps += delta; // delta is rotation angle in 1/8 degree steps
   if ( Abs( m_wheelSteps ) >= WHEEL_STEP_ANGLE*8 )
   {
      if ( delta > 0 )
      {
         int z = m_blink.m_screen.ZoomFactor() - 1;
         m_blink.m_screen.SetZoomFactor( (z != 0) ? z : -2 );
      }
      else
         m_blink.m_screen.ZoomIn();

      m_wheelSteps = 0;

      sender.Update();
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::__ScrollControl_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( m_blink.m_filesData.IsEmpty() )
   {
      TranslucentPlanets();
      sender.Update();
      return;
   }

   if ( buttons != MouseButton::Left )
      return;

   if ( m_blink.m_screen.IsNull() )
      return;

   const double k = double( m_blink.m_info.width )/sender.Width();
   const Point p = m_blink.m_screen.PreviewRect( m_blink.m_screen.CurrentView().Id() ).LeftTop();
   m_blink.m_screen.SetViewport( pos.x*k - p.x, pos.y*k - p.y );

   sender.Update();
}

// ----------------------------------------------------------------------------

void BlinkInterface::__ScrollControl_MouseMove( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( m_blink.m_screen.IsNull() || m_blink.m_filesData.IsEmpty() )
      return;
   if ( buttons != MouseButton::Left )
      return;

   const double k = double( m_blink.m_info.width )/sender.Width() ;
   const Point p = m_blink.m_screen.PreviewRect( m_blink.m_screen.CurrentView().Id() ).LeftTop();
   m_blink.m_screen.SetViewport( pos.x*k - p.x, pos.y*k - p.y );

   sender.Update();
}

// ----------------------------------------------------------------------------

void BlinkInterface::__FilePanelHideButton_Click( Button& sender, bool /*checked*/ )
{
   if ( GUI->RightPanel_Control.IsHidden() )
   {
      SetVariableSize();

      GUI->RightPanel_Control.Show();
      GUI->RightPanel_Control.SetVariableSize();

      GUI->ShowTreeBox_Button.SetIcon( Bitmap( ScaledResource( ":/process-interface/contract.png" ) ) );
      GUI->ShowTreeBox_Button.SetToolTip( "<p>Hide file panel</p>" );
   }
   else
   {
      GUI->RightPanel_Control.SetFixedSize();
      GUI->RightPanel_Control.Hide();

      AdjustToContents();
      SetFixedSize();

      GUI->ShowTreeBox_Button.SetIcon( Bitmap( ScaledResource( ":/process-interface/expand.png" ) ) );
      GUI->ShowTreeBox_Button.SetToolTip( "<p>Show file panel</p>" );
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::__FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->Files_TreeBox.Viewport() )
      wantsFiles = true;
}

// ----------------------------------------------------------------------------

void BlinkInterface::__FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->Files_TreeBox.Viewport() )
   {
      StringList inputFiles;
      bool recursive = IsControlOrCmdPressed();
      for ( const String& item : files )
         if ( File::Exists( item ) )
            inputFiles << item;
         else if ( File::DirectoryExists( item ) )
            inputFiles << FileFormat::SupportedImageFiles( item, true/*toRead*/, false/*toWrite*/, recursive );

      inputFiles.Sort();
      AddFiles( inputFiles );
   }
}

// ----------------------------------------------------------------------------

void BlinkInterface::__Show( Control& /*sender*/ )
{
   // If necessary, generate the preview bitmap
   if ( m_previewBmp.IsNull() )
      GeneratePreview();
   // Enable periodic preview refresh
   GUI->UpdatePreview_Timer.Start();
}

// ----------------------------------------------------------------------------

void BlinkInterface::__Hide( Control& /*sender*/ )
{
   // Stop periodic preview refreshing
   GUI->UpdatePreview_Timer.Stop();
   // Stop animation
   Stop();
}

// ----------------------------------------------------------------------------

void BlinkInterface::__UpdateAnimation_Timer( Timer& timer )
{
   if ( m_blink.CheckScreen() )
   {
      m_blink.ShowNextImage();
      timer.Start();
   }
   else
      Stop();
}

// ----------------------------------------------------------------------------

void BlinkInterface::__UpdatePreview_Timer( Timer& timer )
{
   /*
    * Here we must be careful to prevent unnecessary timer events when the
    * interface is not visible, or when we have no files to animate.
    */
   if ( IsVisible() )
   {
      GUI->Preview_ScrollBox.Viewport().Update();
      if ( !m_blink.m_filesData.IsEmpty() )
         timer.Start();
   }
}

// ----------------------------------------------------------------------------
// Helper routines
// ----------------------------------------------------------------------------

bool BlinkInterface::LoadImage( ImageVariant& image, const String& filePath )
{
   if ( image )
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: return LoadImage_P( static_cast<Image&>( *image ), filePath );
         case 64: return LoadImage_P( static_cast<DImage&>( *image ), filePath );
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: return LoadImage_P( static_cast<UInt8Image&>( *image ), filePath );
         case 16: return LoadImage_P( static_cast<UInt16Image&>( *image ), filePath );
         case 32: return LoadImage_P( static_cast<UInt32Image&>( *image ), filePath );
         }
   return false;
}

// ----------------------------------------------------------------------------

String BlinkInterface::SelectDirectory( const String& caption, const String& initialPath )
{
   GetDirectoryDialog d;
   d.SetCaption( caption );
   d.SetInitialPath( initialPath );
   if ( d.Execute() )
      return d.Directory();
   return String();
}

// ----------------------------------------------------------------------------

String BlinkInterface::UniqueFilePath( const String& fileName, const String& dir )
{
   String filePath = dir;
   if ( !dir.EndsWith( '/' ) )
      filePath << '/';
   filePath << File::ExtractName( fileName ) << File::ExtractExtension( fileName );
   if ( !File::Exists( filePath ) )
      return filePath;

   for ( unsigned u = 1; ; ++u )
   {
      String tryFilePath = File::AppendToName( filePath, '_' + String( u ) );
      if ( !File::Exists( tryFilePath ) )
         return tryFilePath;
   }
}

// ----------------------------------------------------------------------------

int BlinkInterface::FileNumberGet( const int row ) //extract file # from GUI TreeBox
{
   return TheBlinkInterface->GUI->Files_TreeBox.Child( row )->Text( 1 ).ToInt( 10 );
}

String BlinkInterface::RowToStringFileNumber( const int row ) //Convert fileNumber to String with additional "0"
{
   const int fileNumber = FileNumberGet( row );
   String s = String( fileNumber );
   s.Prepend( '0', String( TheBlinkInterface->GUI->Files_TreeBox.NumberOfChildren() ).Length() - s.Length() );
   TheBlinkInterface->GUI->Files_TreeBox.Child( row )->SetText( 2, s );
   return s;
}

// ----------------------------------------------------------------------------

FileFormatInstance BlinkInterface::CreateImageFile( int index, const String& history, const String& dir )
{
   const FileData& fd = m_blink.m_filesData[index];

   String filePath = fd.m_filePath;

   String extension;
   if ( fd.m_format->CanWrite() )
      extension = File::ExtractExtension( filePath );
   else
      extension = ".xisf";

   filePath = UniqueFilePath( File::ChangeExtension( filePath, extension ), dir );
   FileFormat outputFormat( extension, false/*toRead*/, true/*toWrite*/ );
   FileFormatInstance outputFile( outputFormat );
   Console().WriteLn( "<end><cbr>* Creating file: <raw>" + filePath + "</raw>" );
   if ( !outputFile.Create( filePath ) )
      throw CaughtException();

   outputFile.SetOptions( fd.m_options );

   if ( fd.m_format->UsesFormatSpecificData() )
      outputFile.SetFormatSpecificData( fd.m_fsData );

   if ( fd.m_format->CanStoreKeywords() )
   {
      FITSKeywordArray keywords( fd.m_keywords );
      if ( !history.IsEmpty() )
      {
         keywords.Add( FITSHeaderKeyword( "COMMENT", IsoString(), "Processed with " + PixInsightVersion::AsString() ) );
         keywords.Add( FITSHeaderKeyword( "COMMENT", IsoString(), "Processed with " + Module->ReadableVersion() ) );
         keywords.Add( FITSHeaderKeyword( "HISTORY", IsoString(), history ) );
      }
      outputFile.WriteFITSKeywords( keywords );
   }

   if ( fd.m_format->CanStoreICCProfiles() )
      outputFile.WriteICCProfile( fd.m_profile );

   return outputFile;
}

// ----------------------------------------------------------------------------

void BlinkInterface::ResetFilesTreeBox()
{
   GUI->Files_TreeBox.Clear();
   GUI->Files_TreeBox.SetNumberOfColumns( 3 );
   GUI->Files_TreeBox.HideHeader();
   GUI->Files_TreeBox.Sort( 2 );       // sort by fileNumber with preped '0'
   GUI->Files_TreeBox.HideColumn( 1 ); // hidden column to store link from GUI to fileData
   GUI->Files_TreeBox.HideColumn( 2 ); // hidden column to store string representation of link from GUI to fileData
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BlinkInterface::GUIData::GUIData( BlinkInterface& w )
{
   int ri22 = w.LogicalPixelsToResource( 22 );

   //

   const char* scrollControlToolTip =
   "<p>1. Use the mouse wheel to zoom in/out.<br/>"
      "2. Click or click & drag to move the viewing region to a new position.</p>"
   "<p>Note: you can get the same result via direct manipulation (zoom/resize/scroll) on the BilnkScreen image window.</p>";

   Preview_ScrollBox.SetToolTip( scrollControlToolTip );
   Preview_ScrollBox.SetStyle( FrameStyle::Flat ); // no frame
   Preview_ScrollBox.SetScaledFixedSize( PreviewSize, PreviewSize );
   Preview_ScrollBox.Viewport().SetCursor( StdCursor::CirclePlus );
   Preview_ScrollBox.Viewport().OnPaint( (Control::paint_event_handler)&BlinkInterface::__ScrollControl_Paint, w );
   Preview_ScrollBox.Viewport().OnMouseWheel( (Control::mouse_wheel_event_handler)&BlinkInterface::__ScrollControl_MouseWheel, w );
   Preview_ScrollBox.Viewport().OnMousePress( (Control::mouse_button_event_handler)&BlinkInterface::__ScrollControl_MousePress, w );
   Preview_ScrollBox.Viewport().OnMouseMove( (Control::mouse_event_handler)&BlinkInterface::__ScrollControl_MouseMove, w );

   AutoSTF_Button.SetCheckable();
   AutoSTF_Button.SetIcon( Bitmap::FromSVGFile( "@module_icons_dir/../IntensityTransformations/ScreenTransferFunction.svg", ri22, ri22 ) );
   AutoSTF_Button.SetScaledFixedSize( 28, 28 );
   AutoSTF_Button.SetToolTip( "<p>Compute AutoSTF for the curent image, then apply the computed STF to all images.</p>" );
   AutoSTF_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__Brightness_Click, w );

   AutoHT_Button.SetCheckable();
   AutoHT_Button.SetIcon( Bitmap::FromSVGFile( "@module_icons_dir/../IntensityTransformations/HistogramTransformation.svg", ri22, ri22 ) );
   AutoHT_Button.SetScaledFixedSize( 28, 28 );
   AutoHT_Button.SetToolTip( "<p>Apply an automatic histogram transformation to all images</p>" );
   AutoHT_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__Brightness_Click, w );

   RGBLinked_Button.SetCheckable();
   RGBLinked_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/link.png" ) ) );
   RGBLinked_Button.SetScaledFixedSize( 22, 22 );
   RGBLinked_Button.SetToolTip( "<p>Link RGB channels. Enabled only for RGB images.</p>" );
   RGBLinked_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__Brightness_Click, w );

   PreviousImage_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/left.png" ) ) );
   PreviousImage_Button.SetScaledFixedSize( 22, 22 );
   PreviousImage_Button.SetToolTip( "Previous image" );
   PreviousImage_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__ActionButton_Click, w );

   Play_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/play.png" ) ) );
   Play_Button.SetScaledFixedSize( 22, 22 );
   Play_Button.SetToolTip( "Play Animation" );
   Play_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__ActionButton_Click, w );

   NextImage_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/right.png" ) ) );
   NextImage_Button.SetScaledFixedSize( 22, 22 );
   NextImage_Button.SetToolTip( "Next image" );
   NextImage_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__ActionButton_Click, w );

   for ( float d : g_delaySecs )
      BlinkingDelay_ComboBox.AddItem( String().Format( "%.2f sec", d ) );
   BlinkingDelay_ComboBox.SetToolTip( "<p>Minimum delay between successive blinking images.</p>" );
   BlinkingDelay_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&BlinkInterface::__Delay_ItemSelected, w );

   ShowTreeBox_Button.SetIcon( Bitmap( w.ScaledResource( ":/process-interface/contract.png" ) ) );
   ShowTreeBox_Button.SetScaledFixedSize( 22, 22 );
   ShowTreeBox_Button.SetToolTip( "Hide file panel" );
   ShowTreeBox_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FilePanelHideButton_Click, w );

   //

   const char* filesTreeBoxToolTip =
   "<p>1. Only checked images are used for blinking.<br/>"
      "2. To invert the selection for a set of images, select the images and click a check box, or press Space.<br/>"
      "3. Double click to select the BlinkMaster image.</p>";

   const char* fileNameNoteToolTip =
      "<p>Note: If necessary, the source files being copied or moved will be renamed to make them unique in the target folder. "
         "Existing files with the same names as source files will always be preserved.</p>";

   const char* selectionNoteToolTip =
      "<p>Note: <i>Checked</i> is not the same as <i>selected</i>. "
         "To select more then one image use Shift or Ctrl/Cmd + arow keys or click.</p>";

   Files_TreeBox.SetNumberOfColumns( 3 );
   Files_TreeBox.SetScaledMinWidth( 250 );
   Files_TreeBox.HideHeader();
   Files_TreeBox.Sort( 2 ); // sort by fileNumber with preped '0'
   Files_TreeBox.HideColumn( 1 ); // hidden column to store link from GUI to fileData
   Files_TreeBox.HideColumn( 2 ); // hidden column to store string representation of link from GUI to fileData

   Files_TreeBox.EnableMultipleSelections();
   Files_TreeBox.DisableRootDecoration();
   Files_TreeBox.EnableAlternateRowColor();
   Files_TreeBox.SetToolTip( String( filesTreeBoxToolTip ) + selectionNoteToolTip );
   Files_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&BlinkInterface::__Files_NodeSelectionUpdated, w );
   Files_TreeBox.OnNodeUpdated( (TreeBox::node_event_handler)&BlinkInterface::__Files_NodeUpdated, w );
   Files_TreeBox.OnMouseWheel( (Control::mouse_wheel_event_handler)&BlinkInterface::__Files_MouseWheel, w );
   Files_TreeBox.OnNodeDoubleClicked( (TreeBox::node_event_handler)&BlinkInterface::__Files_NodeDoubleClicked, w );
   Files_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&BlinkInterface::__FileDrag, w );
   Files_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&BlinkInterface::__FileDrop, w );

   FileAdd_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/folder-open.png" ) ) );
   FileAdd_Button.SetScaledFixedSize( 22, 22 );
   FileAdd_Button.SetToolTip( "<p>Add image files.</p>" );
   FileAdd_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   FileClose_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/window-close.png" ) ) );
   FileClose_Button.SetScaledFixedSize( 22, 22 );
   FileClose_Button.SetToolTip( String( "<p>Close Selected images.</p>" ) + selectionNoteToolTip );
   FileClose_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   FileCloseAll_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/window-close-all.png" ) ) );
   FileCloseAll_Button.SetScaledFixedSize( 22, 22 );
   FileCloseAll_Button.SetToolTip( "<p>Close all images.</p>" );
   FileCloseAll_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   FileCopyTo_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/save.png" ) ) );
   FileCopyTo_Button.SetScaledFixedSize( 22, 22 );
   FileCopyTo_Button.SetToolTip( String( "<p>Copy selected files to new location.</p>" ) + fileNameNoteToolTip + selectionNoteToolTip );
   FileCopyTo_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   FileMoveTo_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/file-copy.png" ) ) );
   FileMoveTo_Button.SetScaledFixedSize( 22, 22 );
   FileMoveTo_Button.SetToolTip( String( "<p>Move selected files to new location.</p>" ) + fileNameNoteToolTip + selectionNoteToolTip );
   FileMoveTo_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   FileCropTo_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/cut.png" ) ) );
   FileCropTo_Button.SetScaledFixedSize( 22, 22 );
   FileCropTo_Button.SetToolTip(
      String( "<p>Crop the selected files by the green rectangle and save them to a new location. "
            "You can define a preview to use its size and position.</p>" ) + fileNameNoteToolTip + selectionNoteToolTip );
   FileCropTo_Button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   Statistics_button.SetIcon( Bitmap( w.ScaledResource( ":/icons/statistics.png" ) ) );
   Statistics_button.SetScaledFixedSize( 22, 22 );
   Statistics_button.SetToolTip( "<p>Series analysis report.</p>");
   Statistics_button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   CropToVideo_button.SetIcon( Bitmap( w.ScaledResource( ":/icons/clap.png" ) ) );
   CropToVideo_button.SetScaledFixedSize( 22, 22 );
   CropToVideo_button.SetToolTip( "<p>Crop and create video.</p>");
   CropToVideo_button.OnClick( (Button::click_event_handler)&BlinkInterface::__FileButton_Click, w );

   //

   STF_Sizer.SetSpacing( 4 );
   STF_Sizer.Add( AutoHT_Button );
   STF_Sizer.Add( AutoSTF_Button );
   STF_Sizer.AddSpacing( 2 );
   STF_Sizer.Add( RGBLinked_Button );
   STF_Sizer.AddStretch();

   Preview_Sizer.SetSpacing( 4 );
   Preview_Sizer.AddStretch();
   Preview_Sizer.Add( Preview_ScrollBox );
   Preview_Sizer.Add( STF_Sizer );

   ActionControl_Sizer.AddStretch();
   ActionControl_Sizer.Add( PreviousImage_Button );
   ActionControl_Sizer.Add( Play_Button );
   ActionControl_Sizer.Add( NextImage_Button ) ;
   ActionControl_Sizer.AddSpacing( 4 );
   ActionControl_Sizer.Add( BlinkingDelay_ComboBox );
   ActionControl_Sizer.AddSpacing( 4 );
   ActionControl_Sizer.Add( ShowTreeBox_Button );
   ActionControl_Sizer.AddSpacing( 6+1 ); // align ShowTreeBox_Button with RGBLinked_Button

   CentralPanel_Sizer.SetSpacing( 4 );
   CentralPanel_Sizer.Add( Preview_Sizer );
   CentralPanel_Sizer.Add( ActionControl_Sizer );
   CentralPanel_Sizer.AddStretch();

   CentralPanel_Control.SetSizer( CentralPanel_Sizer );

   //

   FilesControl_Sizer.SetSpacing( 4 );
   FilesControl_Sizer.Add( FileAdd_Button );
   FilesControl_Sizer.Add( FileClose_Button );
   FilesControl_Sizer.Add( FileCloseAll_Button );
   FilesControl_Sizer.Add( FileCopyTo_Button );
   FilesControl_Sizer.Add( FileMoveTo_Button );
   FilesControl_Sizer.Add( FileCropTo_Button );
   FilesControl_Sizer.Add( Statistics_button );
   FilesControl_Sizer.Add( CropToVideo_button );
   FilesControl_Sizer.AddStretch();

   RightPanel_Sizer.SetSpacing( 4 );
   RightPanel_Sizer.Add( Files_TreeBox, 100 );
   RightPanel_Sizer.Add( FilesControl_Sizer );

   RightPanel_Control.SetSizer( RightPanel_Sizer );

   //

   Global_Sizer.SetMargin( 4 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( CentralPanel_Control );
   Global_Sizer.Add( RightPanel_Control, 100 );

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();

   w.OnShow( (Control::event_handler)&BlinkInterface::__Show, w );
   w.OnHide( (Control::event_handler)&BlinkInterface::__Hide, w );

   UpdateAnimation_Timer.SetSingleShot();

   int delayItemIndex = int( LinearSearch( g_delaySecs, g_delaySecs+ItemsInArray( g_delaySecs ), 0.05F ) - g_delaySecs );
   if ( delayItemIndex < BlinkingDelay_ComboBox.NumberOfItems() )
   {
      UpdateAnimation_Timer.SetInterval( 0.05 );
      BlinkingDelay_ComboBox.SetCurrentItem( delayItemIndex );
   }
   else
      UpdateAnimation_Timer.SetInterval( 0.0 ); // deliver as soon as the event queue gets empty

   UpdateAnimation_Timer.OnTimer( (Timer::timer_event_handler)&BlinkInterface::__UpdateAnimation_Timer, w );

   UpdatePreview_Timer.SetSingleShot();
   UpdatePreview_Timer.SetInterval( 0.2 );
   UpdatePreview_Timer.OnTimer( (Timer::timer_event_handler)&BlinkInterface::__UpdatePreview_Timer, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BlinkInterface.cpp - Released 2021-04-09T19:41:49Z
