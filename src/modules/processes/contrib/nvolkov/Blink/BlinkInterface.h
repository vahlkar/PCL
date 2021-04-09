//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// BlinkInterface.h - Released 2021-04-09T19:41:49Z
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

#ifndef __BlinkInterface_h
#define __BlinkInterface_h

#include <pcl/ComboBox.h>
#include <pcl/FITSHeaderKeyword.h>
#include <pcl/ICCProfile.h>
#include <pcl/Image.h>
#include <pcl/ImageDescription.h>
#include <pcl/ImageInfo.h>
#include <pcl/ImageOptions.h>
#include <pcl/ImageStatistics.h>
#include <pcl/ImageWindow.h>
#include <pcl/Label.h>
#include <pcl/ReferenceArray.h>
#include <pcl/ProcessInterface.h>
#include <pcl/ScrollBox.h>
#include <pcl/Sizer.h>
#include <pcl/Timer.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FileFormat;
class FileFormatInstance;
class ImageWindow;

typedef UInt16Image  blink_image;

// ----------------------------------------------------------------------------

class BlinkInterface : public ProcessInterface
{
public:

   BlinkInterface();
   virtual ~BlinkInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& ) override;
   void SaveSettings() const override;
   void LoadSettings() override;
   bool WantsImageNotifications() const override;
   void ImageCreated( const View& ) override;
   void ImageDeleted( const View& ) override;

   void SaveVideoSettings() const;
   void LoadVideoSettings();

private:

   typedef Array<ImageStatistics>   stats_list;

   // -------------------------------------------------------------------------

   struct FileData
   {
      String            m_filePath;          // file path of main image
      blink_image*      m_image = nullptr;   // main image
      FileFormat*       m_format = nullptr;  // the file format of retrieved data
      const void*       m_fsData = nullptr;  // format-specific data
      ImageOptions      m_options;
      ImageInfo         m_info;
      FITSKeywordArray  m_keywords;
      ICCProfile        m_profile;
      stats_list        m_statSTF;           // image statistics for STF calculation
      stats_list        m_statReal;          // image statistics in actual pixel values
      Rect              m_statRealRect = 0;  // geometry of statReal
      bool              m_isSTFStatisticsEqualToReal = false; // statSTF generated from RealImage?
      bool              m_isRealPixelData = false; // PixelData in image == RealImage?

      FileData( FileFormatInstance&     file,
                blink_image*            image,
                const ImageDescription& description,
                const String&           filePath,
                bool                    realPixelData );

      ~FileData();
   };

   // -------------------------------------------------------------------------

   struct BlinkData
   {
      BlinkData() = default;

      ~BlinkData()
      {
         Clear();
      }

      bool Add( const String& filePath ); // Load new image from File
      void Remove( int row );             // Remove one FileData record
      void Clear();                       // Remove all FileData records

      void UpdateScreen( int fileNumber );// Regenerate BlinkScreen image window

      void UpdateScreen()
      {
         UpdateScreen( FileNumberGet( m_currentImage ) );
      }

      void Update( int row );             // Update BlinkScreen + GUI

      void Update()
      {
         Update( m_currentImage );
      }

      void Next();                        // Show Next image on BlinkScreen
      void Prev();                        // Show Prev image on BlinkScreen
      void ShowNextImage();

      void GetStatsForSTF( int fileNumber ); // Calculate image statistics

      void GetStatsForSTF()
      {
         GetStatsForSTF( FileNumberGet( m_currentImage ) );
      }

      void AutoSTF();                     // Calculate and set auto screen stretch
      void AutoHT();                      // Calculate and apply auto histogram stretch
      void ResetHT();                     // Recover original pixel data after AutoHT()

      void EnableSTF();
      void DisableSTF();

      bool CheckScreen();                 // Close BlinkScreen if no files, return true if window valid

      bool IsColor() const
      {
         return m_info.colorSpace != ColorSpace::Gray;
      }

      bool CheckGeomery( const ImageDescription& description ); // Compare size of new image with Screen size

      Rect                     m_screenRect = 0;   // BlinkScreen geometry
      Rect                     m_statRect = 0;     // statReal geometry
      ReferenceArray<FileData> m_filesData;        // Path, Images, FITS keywords, ICC profile, ...
      ImageWindow              m_screen = ImageWindow::Null(); // BlinkScreen image window
      ImageInfo                m_info;             // BlinkScreen image info
      ImageOptions             m_options;          // BlinkScreen image options
      int                      m_currentImage = 0; // current image index in m_filesData
      int                      m_blinkMaster = 0;  // Index of blinkMaster
      bool                     m_isBlinkMaster = false;
   };

   // -------------------------------------------------------------------------

   class AutoHTThread : public Thread
   {
   public:

      AutoHTThread( const AbstractImage::ThreadData& data, BlinkData& blinkData, int startIndex, int endIndex )
         : m_data( data )
         , m_blinkData( blinkData )
         , m_startIndex( startIndex )
         , m_endIndex( endIndex )
      {
      }

      void Run() override;

   private:

      const AbstractImage::ThreadData& m_data;
            BlinkData&                 m_blinkData;
            int                        m_startIndex;
            int                        m_endIndex;
   };

   // -------------------------------------------------------------------------

   struct GUIData
   {
      GUIData( BlinkInterface& );

      HorizontalSizer   Global_Sizer;

      Control              CentralPanel_Control;
      VerticalSizer        CentralPanel_Sizer;
         HorizontalSizer      Preview_Sizer;
            ScrollBox            Preview_ScrollBox;
            VerticalSizer        STF_Sizer;
               ToolButton           AutoSTF_Button;
               ToolButton           AutoHT_Button;
               ToolButton           RGBLinked_Button;

         HorizontalSizer         ActionControl_Sizer;
            ToolButton              PreviousImage_Button;
            ToolButton              Play_Button;
            ToolButton              NextImage_Button;
            ComboBox                BlinkingDelay_ComboBox;
            ToolButton              ShowTreeBox_Button;

      Control              RightPanel_Control;
      VerticalSizer        RightPanel_Sizer;
         TreeBox              Files_TreeBox;
         HorizontalSizer      FilesControl_Sizer;
            ToolButton           FileAdd_Button;
            ToolButton           FileClose_Button;
            ToolButton           FileCloseAll_Button;
            ToolButton           FileCopyTo_Button;
            ToolButton           FileMoveTo_Button;
            ToolButton           FileCropTo_Button;
            ToolButton           Statistics_button;
            ToolButton           CropToVideo_button;

      Timer UpdateAnimation_Timer;
      Timer UpdatePreview_Timer;
   };

   // -------------------------------------------------------------------------

   GUIData*    GUI = nullptr;
   BlinkData   m_blink;
   Bitmap      m_previewBmp = Bitmap::Null();   // preview image
   bool        m_isRunning = false;       // true = show is going
   int         m_wheelSteps = 0;          // accumulated 1/8-degree wheel steps

   /*
    * Statistics default settings
    */
   bool        m_sortChannels = true;     // mode: sort by channel(true) or not(false)?
   bool        m_cropMode = false;        // true = Statistics only for Green rectangle
   int         m_digits0_1 = 6;           // digits after dot for range 0-1.
   int         m_digits0_65535 = 3;       // digits after dot for range 0-65535.
   bool        m_range0_65535 = true;     // true = use range range 0-65535. false = use range range 0-1.
   bool        m_writeStatsFile = false;  // true = write a text file with statistical data

   /*
    * Video generation settings
    */
   String      m_outputFileName;          // file name for output video files
   String      m_program;                 // execute the program
   String      m_arguments;               // execute with parameters
   String      m_frameOutputDir;          // output directory for frames and video
   String      m_frameExtension = ".png"; // file extension for video frames
   bool        m_deleteVideoFrames = false; // delete or not temporary video frame files

   /*
    * Worker routines
    */
   void Init();                  // enable/disable buttons
   void TranslucentPlanets();    // nice pic to Preview if no files open
   void Image2Preview();
   void GeneratePreview();

   void Play();
   void Stop();
   void Pause();
   void Continue();
   void DisableButtonsIfRunning();

   void FileAdd();
   void FileCloseSelected();
   void FileCloseAll();
   void FileCopyTo();
   void FileMoveTo();
   void CropToVideo();
   void FileCropTo();
   void FileStatistics();

   void UpdateFileNumbers();
   void FileNumberSet( const int row, const int fileNumber );  // set fileNumber in GUI TreeBox row

   /*
    * Event handlers
    */
   void __Brightness_Click( Button&, bool );
   void __Files_NodeSelectionUpdated( TreeBox& );
   void __Files_NodeUpdated( TreeBox&, TreeBox::Node&, int );
   void __Files_MouseWheel( Control&, const Point&, int, unsigned, unsigned );
   void __Files_NodeDoubleClicked( TreeBox&, TreeBox::Node&, int );
   void __FileButton_Click( Button&, bool );
   void __Delay_ItemSelected( ComboBox&, int );
   void __ActionButton_Click( Button&, bool );
   void __ScrollControl_Paint( Control&, const Rect& );
   void __ScrollControl_MouseWheel( Control&, const Point&, int, unsigned, unsigned );
   void __ScrollControl_MousePress( Control&, const Point&, int, unsigned, unsigned );
   void __ScrollControl_MouseMove( Control&, const Point&, unsigned, unsigned );
   void __FilePanelHideButton_Click( Button&, bool );
   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );
   void __Show( Control& );
   void __Hide( Control& );
   void __UpdateAnimation_Timer( Timer& );
   void __UpdatePreview_Timer( Timer& );

   /*
    * Helper routines
    */
   static bool LoadImage( ImageVariant&, const String& filePath );
   static String SelectDirectory( const String& caption, const String& initialPath = String() );
   static String UniqueFilePath( const String& fileName, const String& dir );
   static int FileNumberGet( const int row );   // extract fileNumber from GUI TreeBox
   static String RowToStringFileNumber( const int row ); // convert fileNumber to String with additional "0"

   FileFormatInstance CreateImageFile( int index, const String& history, const String& dir );

   void AddFiles( const StringList& );
   void ResetFilesTreeBox();
   Rect GetCropRect(); //return geometry of sub window

   //

   friend struct BlinkInterface::GUIData;
   friend struct BlinkInterface::BlinkData;
   friend class  BlinkStatisticsDialog;
   friend class  BlinkVideoDialog;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern BlinkInterface* TheBlinkInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------
} // pcl

#endif   // __BlinkInterface_h

// ----------------------------------------------------------------------------
// EOF BlinkInterface.h - Released 2021-04-09T19:41:49Z
