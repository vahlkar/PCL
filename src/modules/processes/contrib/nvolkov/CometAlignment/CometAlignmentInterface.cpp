//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.4
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 1.3.8
// ----------------------------------------------------------------------------
// CometAlignmentInterface.cpp - Released 2024-12-23T11:33:28Z
// ----------------------------------------------------------------------------
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2023 Nikolay Volkov
// Copyright (c) 2019-2023 Juan Conejero (PTeam)
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L.
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

#include "CentroidDetector.h"
#include "CometAlignmentInterface.h"
#include "CometAlignmentProcess.h"

#include <pcl/DrizzleData.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/Graphics.h>
#include <pcl/MessageBox.h>
#include <pcl/ProgressBarStatus.h>
#include <pcl/StandardStatus.h>

#define IMAGELIST_MINHEIGHT( fnt ) RoundInt( 8.125 * fnt.Height() )

namespace pcl
{

// ----------------------------------------------------------------------------

CometAlignmentInterface* TheCometAlignmentInterface = nullptr;

/*
 * Constants for dynamic painting.
 */
const int s_bigCircleRadius   = 33;
const int s_smallCircleRadius =  4;
const int s_centerMarkSize    =  5;

// ----------------------------------------------------------------------------

CometAlignmentInterface::CometAlignmentInterface()
   : m_instance( TheCometAlignmentProcess )
{
   TheCometAlignmentInterface = this;
   DisableAutoSaveGeometry();
}

// ----------------------------------------------------------------------------

CometAlignmentInterface::~CometAlignmentInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString CometAlignmentInterface::Id() const
{
   return "CometAlignment";
}

// ----------------------------------------------------------------------------

MetaProcess* CometAlignmentInterface::Process() const
{
   return TheCometAlignmentProcess;
}

// ----------------------------------------------------------------------------

String CometAlignmentInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/CometAlignment.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures CometAlignmentInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::ResetInstance()
{
   CometAlignmentInstance defaultInstance( TheCometAlignmentProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool CometAlignmentInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "CometAlignment" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = true;
   return &P == TheCometAlignmentProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* CometAlignmentInterface::NewProcess() const
{
   return new CometAlignmentInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool CometAlignmentInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const CometAlignmentInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a CometAlignment instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool CometAlignmentInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool CometAlignmentInterface::ImportProcess( const ProcessImplementation& p )
{
   RedrawAll();
   m_instance.Assign( p );
   UpdateControls();
   RedrawAll();
   return true;
}

// ----------------------------------------------------------------------------

bool CometAlignmentInterface::IsDynamicInterface() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool CometAlignmentInterface::EnterDynamicMode()
{
   /*
    * Restore all dynamic drawings.
    */
   RedrawAll();
   return true;
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::ExitDynamicMode()
{
   /*
    * Clear all dynamic drawings.
    */
   RedrawAll();
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::DynamicMousePress( View& view, const DPoint& pos, int button, unsigned buttons, unsigned modifiers )
{
   const String filePath = view.Window().FilePath();
   for ( size_type index = 0; index < m_instance.p_targets.Length(); ++index )
   {
      const CometAlignmentInstance::ImageItem& item = m_instance.p_targets[index];
      if ( item.path == filePath )
      {
         DPoint position( pos );                   // initialize to mouse click x,y position
         if ( modifiers != KeyModifier::Control )  // Ctrl modifier disables automatic centroid detection
         {
            Console console;
            ImageVariant image = view.Image();
            Image I;
            ImageVariant V;
            if ( image.IsColor() )
            {
               V = ImageVariant( &I );
               image.GetIntensity( V );
            }
            else
               V = image;

            CentroidDetector D( V, 0, pos );
            if ( D )
            {
               position = D.psf.c0;
               console.WriteLn( "<end><cbr>PSF: "
                        + D.psf.FunctionName()
                        + String().Format( ", B = %.6f, A = %.6f, FWHMx = %.2f, FWHMy = %.2f,<br>     r = %.3f, theta = %.2f, beta = %.2f",
                                           D.psf.B, D.psf.A, D.psf.FWHMx(), D.psf.FWHMy(), D.psf.sy/D.psf.sx, D.psf.theta, D.psf.beta ) );
               console.WriteLn( String().Format( "cx = %.2f, cy = %.2f", position.x, position.y ) );
               if ( item.position != 0.0 )
                  console.WriteLn( String().Format( "<end><cbr>ex = %+.2f, ey = %+.2f",
                                                    position.x - item.position.x, position.y - item.position.y ) );
            }
            else
            {
               console.Show();
               console.CriticalLn( "<end><cbr>** Error: PSF fitting error: " + D.StatusText() + '.' );
               return;
            }
         }

         if ( view.IsPreview() )
            position.MoveBy( view.Window().PreviewRect( view.Id() ).LeftTop() );

         SetFramePosition( index, position );
      }
   }

   view.Window().UpdateViewport(); // the core will call RequiresDynamicUpdate()
}

// ----------------------------------------------------------------------------

static
Rect ViewportSourceSampleRect( const DPoint pos, const ImageWindow window )
{
   Point p = pos.Truncated();
   Rect scr( p - s_smallCircleRadius, p + s_smallCircleRadius + 1 );
   int bcr = TruncInt( Ceil( s_bigCircleRadius * window.DisplayPixelRatio() ) );

   DPoint c1( int( pos.x ) + 0.5, int( pos.y ) + 0.5 );
   window.ImageToViewport( c1.x, c1.y );
   Point c0( int( c1.x ), int( c1.y ) );

   Rect vsr = window.ImageToViewport( scr ).InflatedBy( 1 ); // rounding error compensation
   return vsr.Union( Rect( c0 - bcr, c0 + bcr + 1 ) );
}

bool CometAlignmentInterface::RequiresDynamicUpdate( const View& view, const DRect& updateRect ) const
{
   if ( m_instance.p_targets.IsEmpty() )
      return false;

   ImageWindow window = view.Window();
   const String path = window.FilePath();
   for ( const CometAlignmentInstance::ImageItem& item :  m_instance.p_targets )
   {
      if ( item.path != path )
         continue; // not this image, try next target

      m_imagePos = item.position;
      if ( m_imagePos == 0.0 )
         return false; // centroid position has not been selected by the user

      Rect r = window.ImageToViewport( updateRect ).InflatedBy( 1 ); // rounding error compensation
      if ( r.Intersects( ViewportSourceSampleRect( m_imagePos, window ) ) )
         return true; // update required
   }
   return false; // no such View in p_targets, so no need to update
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::DynamicPaint( const View& view, VectorGraphics& g, const DRect& updateRect ) const
{
   ImageWindow window = view.Window();
   double penWidth = window.DisplayPixelRatio();
   g.EnableAntialiasing();
   g.SetCompositionOperator( CompositionOp::Difference );
   g.SetPen( 0xff00ff00, penWidth ); // green
   g.SetBrush( Brush::Null() );


   // Center position
   DPoint pv0 = window.ImageToViewport( m_imagePos ); // center position

   // Big circle, unscaled
   g.StrokeCircle( pv0, s_bigCircleRadius*penWidth );

   // Small circle, scaled
   double rv = window.ImageScalarToViewport( s_smallCircleRadius );
   if ( rv > 0.5 )
      g.DrawCircle( pv0, rv );

   // Center cross mark, unscaled
   double dr = penWidth * s_centerMarkSize;
   g.DrawLine( pv0.x-dr, pv0.y, pv0.x+dr, pv0.y );
   g.DrawLine( pv0.x, pv0.y-dr, pv0.x, pv0.y+dr );
}

//-------------------------------------------------------------------------

void CometAlignmentInterface::SaveSettings() const
{
   SaveGeometry();
}

//-------------------------------------------------------------------------

/*
 * Open an image, if not already open, and bring the image window to front.
 */
static void OpenAndActivateImageWindow( const String& filePath )
{
   if ( ImageWindow::WindowByFilePath( filePath ).IsNull() )
   {
      Array<ImageWindow> windows = ImageWindow::Open( filePath );
      for ( ImageWindow& window : windows )
         window.Show();
   }
   else
      ImageWindow::WindowByFilePath( filePath ).BringToFront();
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::SetFramePosition( size_type index, const DPoint& pos )
{
   if ( index < m_instance.p_targets.Length() )
   {
      m_instance.p_targets[index].position = pos;
      m_instance.p_targets[index].fixed = true;
   }

   UpdateTargetImagesList();
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::ResetFramePositions()
{
   if ( !m_instance.p_targets.IsEmpty() )
   {
      RedrawAll();
      for ( CometAlignmentInstance::ImageItem& item : m_instance.p_targets )
      {
         item.position = 0;
         item.fixed = false;
      }
      m_instance.p_targets[0].fixed = true;
      m_instance.p_targets[m_instance.p_targets.UpperBound()].fixed = true;
      UpdateTargetImagesList();
      RedrawAll();
   }
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::RedrawAll()
{
   for ( const CometAlignmentInstance::ImageItem& item : m_instance.p_targets )
   {
      ImageWindow window = ImageWindow::WindowByFilePath( item.path );
      if ( !window.IsNull() )
         window.UpdateViewport();
   }
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::SetReferenceFrame( size_type index )
{
   if ( m_instance.p_referenceIndex != index )
   {
      if ( !m_instance.p_targets.IsEmpty() )
      {
         GUI->TargetImages_TreeBox.Child( int( m_instance.p_referenceIndex ) )->SetIcon( 0, Bitmap() );
         GUI->TargetImages_TreeBox.Child( int( m_instance.p_referenceIndex = index ) )->SetIcon( 0, Bitmap( ScaledResource( ":/icons/arrow-right.png" ) ) );
      }

      UpdateTargetImagesList();
   }
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::UpdateSubtractSection()
{
   bool haveOperandImage = !m_instance.p_operandImageFilePath.IsEmpty();

   GUI->SubtractFile_ShowButton.Enable( haveOperandImage );
   GUI->SubtractFile_ClearButton.Enable( haveOperandImage );
   GUI->SubtractComet_RadioButton.Enable( haveOperandImage );
   GUI->SubtractStars_RadioButton.Enable( haveOperandImage );
   GUI->Normalize_CheckBox.Enable( haveOperandImage );
   GUI->LinearFit_CheckBox.Enable( haveOperandImage );
   GUI->RejectLow_NumericControl.Enable( haveOperandImage );
   GUI->RejectHigh_NumericControl.Enable( haveOperandImage );

   GUI->DrzSaveSA_CheckBox.Enable( m_drizzle && haveOperandImage );
   GUI->DrzSaveCA_CheckBox.Enable( m_drizzle && haveOperandImage );
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::UpdateControls()
{
   GUI->InputHints_Edit.SetText( m_instance.p_inputHints );
   GUI->OutputHints_Edit.SetText( m_instance.p_outputHints );
   GUI->OutputDir_Edit.SetText( m_instance.p_outputDirectory );
   GUI->Postfix_Edit.SetText( m_instance.p_outputPostfix );
   GUI->Prefix_Edit.SetText( m_instance.p_outputPrefix );
   GUI->Overwrite_CheckBox.SetChecked( m_instance.p_overwriteExistingFiles );
   GUI->CometPathMap_CheckBox.SetChecked( m_instance.p_generateCometPathMap );

   GUI->PSFFits_CheckBox.SetChecked( m_instance.p_fitPSF );

   GUI->PixelInterpolation_ComboBox.SetCurrentItem( m_instance.p_pixelInterpolation );

   GUI->ClampingThreshold_NumericControl.SetValue( m_instance.p_linearClampingThreshold );
   GUI->ClampingThreshold_NumericControl.Enable(
      m_instance.p_pixelInterpolation == CAPixelInterpolation::BicubicSpline || m_instance.p_pixelInterpolation == CAPixelInterpolation::Lanczos3 || m_instance.p_pixelInterpolation == CAPixelInterpolation::Lanczos4 || m_instance.p_pixelInterpolation == CAPixelInterpolation::Lanczos5 );

   GUI->SubtractFile_Edit.SetText( m_instance.p_operandImageFilePath );

   GUI->SubtractComet_RadioButton.SetChecked( m_instance.p_operandSubtractAligned );
   GUI->SubtractStars_RadioButton.SetChecked( !m_instance.p_operandSubtractAligned );
   GUI->DrzSaveSA_CheckBox.SetChecked( m_instance.p_drizzleWriteStarAlignedImage );
   GUI->DrzSaveCA_CheckBox.SetChecked( m_instance.p_drizzleWriteCometAlignedImage );
   GUI->Normalize_CheckBox.SetChecked( m_instance.p_operandNormalize );
   GUI->LinearFit_CheckBox.SetChecked( m_instance.p_operandLinearFit );
   GUI->RejectLow_NumericControl.SetValue( m_instance.p_operandLinearFitLow );
   GUI->RejectHigh_NumericControl.SetValue( m_instance.p_operandLinearFitHigh );

   UpdateTargetImagesList();
   UpdateImageSelectionButtons();

   UpdateSubtractSection();
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::UpdateTargetImageItem( size_type index )
{
   TreeBox::Node* node = GUI->TargetImages_TreeBox[int( index )];
   if ( node == nullptr )
      return;

   CometAlignmentInstance::ImageItem& item = m_instance.p_targets[index];

   if ( !item.fixed )
   {
      // Find the previous frame with fixed position at index j.
      size_type j = index;
      while ( j > 0 )
         if ( m_instance.p_targets[--j].fixed )
            break;
      const CometAlignmentInstance::ImageItem& itemj = m_instance.p_targets[j];

      if ( itemj.position != 0.0 ) // if a previous fixed position has been defined
      {
         size_type k = index;
         while ( k < m_instance.p_targets.UpperBound() )
            if ( m_instance.p_targets[++k].fixed )
               break;
         const CometAlignmentInstance::ImageItem& itemk = m_instance.p_targets[k];

         if ( itemk.position != 0.0 ) // if a posterior fixed position has been defined
         {
            item.position = itemj.position;
            if ( itemk.jd != itemj.jd )
               item.position += (itemk.position - itemj.position) * ((item.jd - itemj.jd)/(itemk.jd - itemj.jd));
         }
      }
   }

   if ( index == m_instance.p_referenceIndex )
      node->SetIcon( 0, Bitmap( ScaledResource( ":/icons/arrow-right.png" ) ) );
   node->SetToolTip( 0, "Double Click to set reference" );
   node->SetText( 0, String( index + 1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetToolTip( 1, "Double Click to check/uncheck" );
   node->SetAlignment( 1, TextAlign::Left );

   node->SetIcon( 2, item.fixed ? Bitmap( ScaledResource( ":/icons/pin.png" ) ) : Bitmap() );

   node->SetIcon( 3, Bitmap( ScaledResource( ":/browser/document.png" ) ) );
   String fileText;
   if ( !item.drzPath.IsEmpty() )
      fileText = "<d> ";
   else
      m_drizzle = false; // disable drizzle because not all images have .dzr file
   if ( GUI->FullPaths_CheckBox.IsChecked() )
      fileText.Append( item.path );
   else
      fileText.Append( File::ExtractNameAndSuffix( item.path ) );
   node->SetText( 3, fileText );

   String toolTip = "Double-click to open image: " + item.path;

   if ( !item.drzPath.IsEmpty() )
   {
      toolTip.Append( '\n' );
      toolTip.Append( item.drzPath );
   }
   node->SetToolTip( 3, toolTip );
   node->SetAlignment( 3, TextAlign::Left );

   node->SetText( 4, TimePoint( item.jd ).ToString( ISO8601ConversionOptionsNoTimeZone() ) );

   node->SetText( 5, String().Format( "%.2f", item.position.x ) );
   node->SetText( 6, String().Format( "%.2f", item.position.y ) );

   // Columns relative to the reference frame
   const CometAlignmentInstance::ImageItem& itemr = m_instance.p_targets[m_instance.p_referenceIndex];
   node->SetText( 7, String().Format( "%+.3f", (item.jd - itemr.jd) * 86400 ) ); // 86400 seconds per day
   node->SetText( 8, String().Format( "%+.2f", item.position.x - itemr.position.x ) );
   node->SetText( 9, String().Format( "%+.2f", item.position.y - itemr.position.y ) );

   if ( item.position != 0.0 ) // if position has been defined
   {
      ImageWindow window = ImageWindow::WindowByFilePath( item.path );
      if ( !window.IsNull() )
         window.UpdateViewport();
   }
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::UpdateTargetImagesList()
{
   GUI->TargetImages_TreeBox.DisableUpdates();
   GUI->TargetImages_TreeBox.Clear();

   m_drizzle = false;
   if ( !m_instance.p_targets.IsEmpty() )
   {
      m_drizzle = true; // UpdateTargetImageItem() will disable if appropriate
      for ( size_type i = 0; i < m_instance.p_targets.Length(); ++i )
      {
         new TreeBox::Node( GUI->TargetImages_TreeBox );
         UpdateTargetImageItem( i );
      }

      const CometAlignmentInstance::ImageItem& item0 = m_instance.p_targets[0];
      const CometAlignmentInstance::ImageItem& item1 = m_instance.p_targets[m_instance.p_targets.UpperBound()];

      GUI->X0_NumericEdit.SetValue( item0.position.x );
      GUI->Y0_NumericEdit.SetValue( item0.position.y );

      GUI->X1_NumericEdit.SetValue( item1.position.x );
      GUI->Y1_NumericEdit.SetValue( item1.position.y );

      double dt = item1.jd - item0.jd;
      if ( dt != 0 )
      {
         GUI->XDot_NumericEdit.SetValue( (item1.position.x - item0.position.x) / dt / 24 ); // pixels per hour
         GUI->YDot_NumericEdit.SetValue( (item1.position.y - item0.position.y) / dt / 24 );
      }
      else
      {
         GUI->XDot_NumericEdit.SetValue( 0 );
         GUI->YDot_NumericEdit.SetValue( 0 );
      }
   }
   else
   {
      GUI->X0_NumericEdit.SetValue( 0 );
      GUI->Y0_NumericEdit.SetValue( 0 );

      GUI->X1_NumericEdit.SetValue( 0 );
      GUI->Y1_NumericEdit.SetValue( 0 );

      GUI->XDot_NumericEdit.SetValue( 0 );
      GUI->YDot_NumericEdit.SetValue( 0 );
   }

   for ( int i = 0; i < GUI->TargetImages_TreeBox.NumberOfColumns(); ++i )
      GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( i );

   GUI->TargetImages_TreeBox.EnableUpdates();
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::UpdateImageSelectionButtons()
{
   bool hasItems = GUI->TargetImages_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->TargetImages_TreeBox.HasSelectedTopLevelNodes();

   GUI->AddDrizzleFiles_PushButton.Enable( hasItems );
   GUI->ClearDrizzleFiles_PushButton.Enable( hasItems );
   GUI->SetReference_PushButton.Enable( GUI->TargetImages_TreeBox.CurrentNode() != nullptr );
   GUI->SelectAll_PushButton.Enable( hasItems );
   GUI->InvertSelection_PushButton.Enable( hasItems );
   GUI->Clear_PushButton.Enable( hasItems );

   GUI->ToggleSelected_PushButton.Enable( hasSelection );
   GUI->RemoveSelected_PushButton.Enable( hasSelection );

   GUI->X0_NumericEdit.Enable( hasItems );
   GUI->Y0_NumericEdit.Enable( hasItems );
   GUI->X1_NumericEdit.Enable( hasItems );
   GUI->Y1_NumericEdit.Enable( hasItems );

   GUI->ShowFirstFrame_PushButton.Enable( hasItems );
   GUI->ShowLastFrame_PushButton.Enable( hasItems );
   GUI->ResetPositions_PushButton.Enable( hasItems );
}

//-------------------------------------------------------------------------------------

void CometAlignmentInterface::SelectDir()
{
   GetDirectoryDialog d;
   d.SetCaption( "CometAlignment: Select Output Directory" );
   if ( d.Execute() )
      GUI->OutputDir_Edit.SetText( m_instance.p_outputDirectory = d.Directory() );
}

// ----------------------------------------------------------------------------

bool OperandIsDrizzleIntegration( const String& filePath ) //true == DrizzleIntegration, false == ImageIntegration,
{
   bool ret( false );
   FileFormat format( File::ExtractExtension( filePath ), true, false );
   FileFormatInstance file( format );
   ImageDescriptionArray images;
   if ( !file.Open( images, filePath ) )
      throw CaughtException();
   FITSKeywordArray keywords; // FITS keywords
   if ( format.CanStoreKeywords() )
      if ( !file.ReadFITSKeywords( keywords ) )
         throw CaughtException();
      else
         for ( int k = 0; k < int( keywords.Length() ); k++ )
            if ( String( keywords[k].comment ) == "Integration with DrizzleIntegration process" )
               ret = true;
   if ( !file.Close() )
      throw CaughtException();
   return ret;
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::SelectSubtractFile()
{
   OpenFileDialog d;
   d.LoadImageFilters();
   d.DisableMultipleSelections();
   d.SetCaption( "CometAlignment: Select Operand Image" );
   if ( d.Execute() )
      GUI->SubtractFile_Edit.SetText( m_instance.p_operandImageFilePath = d.FileName() );
   UpdateSubtractSection();
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::ShowSubtractFile()
{
   if ( !m_instance.p_operandImageFilePath.IsEmpty() )
      OpenAndActivateImageWindow( m_instance.p_operandImageFilePath );
}

// ----------------------------------------------------------------------------

static String AcquisitionTimeAsISO8601( const String& filePath )
{
   String extension = File::ExtractExtension( filePath );
   FileFormat format( extension, true/*read*/, false/*write*/ );

   FileFormatInstance file( format );
   ImageDescriptionArray images;
   if ( !file.Open( images, filePath ) )
      throw CaughtException();
   if ( images.IsEmpty() )
      throw Error( filePath + ": Empty image." );

   if ( format.CanStoreImageProperties() )
   {
      PropertyArray properties = file.ReadImageProperties();
      for ( auto p : properties )
         if ( p.Id() == "Observation:Time:Start" )
         {
            file.Close();
            return p.Value().ToString();
         }
   }

   if ( format.CanStoreKeywords() )
   {
      FITSKeywordArray keywords;
      file.ReadFITSKeywords( keywords );
      file.Close();
      for ( const FITSHeaderKeyword& keyword : keywords )
         if ( keyword.name == "DATE-OBS" )
            return keyword.StripValueDelimiters();
   }

   return String();
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= m_instance.p_targets.Length() )
      throw Error( "CometAlignmentInterface: *Warning* Corrupted interface structures" );
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   int index = sender.ChildIndex( &node );
   if ( index < 0 || size_type( index ) >= m_instance.p_targets.Length() )
      throw Error( "CometAlignmentInterface: *Warning* Corrupted interface structures" );

   CometAlignmentInstance::ImageItem& item = m_instance.p_targets[index];

   switch ( col )
   {
   case 0:
      // Activate the item's index number: set Reference.
      SetReferenceFrame( index );
      break;
   case 1:
      // Activate the item's checkmark: toggle item's enabled state.
      item.enabled = !item.enabled;
      UpdateTargetImageItem( index );
      break;
   default:
   case 2:
      // Activate the item's path: open the image.
      OpenAndActivateImageWindow( item.path );
      break;
   }
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::e_NodeSelectionUpdated( TreeBox& sender )
{
   UpdateImageSelectionButtons();
}

// ----------------------------------------------------------------------------

String CometAlignmentInterface::DrizzleTargetName( const String& filePath )
{
   DrizzleData drz( filePath, DrizzleParserOption::IgnoreIntegrationData );

   /*
    * If the drizzle file includes a target alignment path, use it. Otherwise
    * the target should have the same name as the drizzle data file.
    */
   String targetfilePath = drz.AlignmentTargetFilePath();
   if ( targetfilePath.IsEmpty() )
      targetfilePath = filePath;

   //    if ( GUI->StaticDrizzleTargets_CheckBox.IsChecked() )
   //       return File::ChangeExtension( targetfilePath, String() );
   return File::ExtractName( targetfilePath );
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::AddFiles( const StringList& filePaths )
{
   size_type N = filePaths.Length();

   Console console;
   console.Show();
   console.NoteLn( "<end><cbr><br><br>* CometAlignment: Loading " + String( N ) + " file(s)" );

   ProgressBarStatus status( "CometAlignment"
#ifdef __PCL_MACOSX
      , *this
#endif
      );
   StatusMonitor monitor;
   monitor.SetCallback( &status );
   monitor.Initialize( "Loading data...", N );

   RedrawAll();

   for ( const String& filePath : filePaths )
   {
      // Skip files which are already in the list
      size_type j = 0;
      for ( ; j < m_instance.p_targets.Length(); ++j )
         if ( m_instance.p_targets.At( j )->path == filePath )
            break;
      if ( j < m_instance.p_targets.Length() )
         continue; // filePath already in list -> skip file

      String date = AcquisitionTimeAsISO8601( filePath );
      if ( date.IsEmpty() )
      {
         console.CriticalLn( "*** Error: Acquisition time metadata not available "
                             "(Observation:Time:Start property or DATE-OBS keyword): <raw>" + filePath + "</raw>" );
         continue;
      }

      m_instance.p_targets << CometAlignmentInstance::ImageItem( filePath, date );

      ++monitor;
   }

   m_instance.p_targets.Sort();

   ResetFramePositions();
   UpdateTargetImagesList();
   UpdateImageSelectionButtons();
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::e_TargetFrames_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.LoadImageFilters();
      d.EnableMultipleSelections();
      d.SetCaption( "CometAlignment: Select Target Frames" );
      if ( d.Execute() )
         AddFiles( d.FileNames() );
   }
   else if ( sender == GUI->SetReference_PushButton )
   {
      TreeBox::Node* node = GUI->TargetImages_TreeBox.CurrentNode();
      if ( node != nullptr )
      {
         int index = GUI->TargetImages_TreeBox.ChildIndex( node );
         SetReferenceFrame( index );
      }
   }
   else if ( sender == GUI->AddDrizzleFiles_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "CometAlignment: Select Drizzle Data Files" );
      d.SetFilter( FileFilter( "Drizzle Data Files", StringList() << ".xdrz" << ".drz" ) );
      d.EnableMultipleSelections();
      if ( d.Execute() )
      {
         IVector assigned( 0, int( m_instance.p_targets.Length() ) );
         for ( const String& filePath : d.FileNames() )
         {
            String targetName = DrizzleTargetName( filePath );
            IVector::iterator n = assigned.Begin();
            for ( CometAlignmentInstance::ImageItem& item : m_instance.p_targets )
            {
               String name = File::ExtractName( item.path );
               if ( name == targetName )
               {
                  item.drzPath = filePath;
                  ++*n;
                  break;
               }
               ++n;
            }
         }

         UpdateTargetImagesList();

         int total = 0;
         int duplicates = 0;
         for ( int n : assigned )
            if ( n > 0 )
            {
               ++total;
               if ( n > 1 )
                  ++duplicates;
            }

         if ( total == 0 )
         {
            MessageBox( "<p>No drizzle data files have been assigned to source images.</p>",
               "CometAlignment",
               StdIcon::Error,
               StdButton::Ok ).Execute();
         }
         else
         {
            if ( total < assigned.Length() || duplicates )
               MessageBox( String().Format( "<p>%d of %d drizzle data files have been assigned.<br/>"
                                            "%d duplicate assignment(s)</p>",
                              total, assigned.Length(), duplicates ),
                  "CometAlignment",
                  StdIcon::Warning,
                  StdButton::Ok ).Execute();
         }
      }
   }
   else if ( sender == GUI->ClearDrizzleFiles_PushButton )
   {
      for ( CometAlignmentInstance::ImageItem& item : m_instance.p_targets )
         item.drzPath.Clear();
      UpdateTargetImagesList();
   }
   else if ( sender == GUI->SelectAll_PushButton )
   {
      GUI->TargetImages_TreeBox.SelectAllNodes();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->InvertSelection_PushButton )
   {
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->TargetImages_TreeBox[i]->Select( !GUI->TargetImages_TreeBox[i]->IsSelected() );
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->ToggleSelected_PushButton )
   {
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->TargetImages_TreeBox[i]->IsSelected() )
            m_instance.p_targets[i].enabled = !m_instance.p_targets[i].enabled;
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->RemoveSelected_PushButton )
   {
      RedrawAll();
      CometAlignmentInstance::image_list newTargets;
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
      {
         if ( !GUI->TargetImages_TreeBox[i]->IsSelected() )
         {
            newTargets.Add( m_instance.p_targets[i] );
            if ( i == int( m_instance.p_referenceIndex ) ) // set new reference pointer
               m_instance.p_referenceIndex = newTargets.Length() - 1;
         }
      }
      m_instance.p_targets = newTargets;

      if ( !m_instance.p_targets.IsEmpty() )
         if ( m_instance.p_referenceIndex >= m_instance.p_targets.Length() ) // reference was removed?
            m_instance.p_referenceIndex = m_instance.p_targets.Length() - 1; // set reference to last image

      ResetFramePositions();
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
      RedrawAll();
   }
   else if ( sender == GUI->Clear_PushButton )
   {
      RedrawAll();
      m_instance.p_referenceIndex = 0;
      m_instance.p_targets.Clear();
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->FullPaths_CheckBox )
   {
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::e_ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
   {
      GUI->TargetImages_TreeBox.SetFixedHeight();
   }
   else
   {
      GUI->TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( Font() ) );
      GUI->TargetImages_TreeBox.SetMaxHeight( int_max );
   }
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::e_EditCompleted( Edit& sender )
{
   if ( !sender.IsModified() )
      return;
   sender.SetModified( false );

   String text = sender.Text().Trimmed();

   if ( sender == GUI->Prefix_Edit )
      m_instance.p_outputPrefix = text;
   else if ( sender == GUI->Postfix_Edit )
      m_instance.p_outputPostfix = text;
   else if ( sender == GUI->OutputDir_Edit )
      m_instance.p_outputDirectory = text;
   else if ( sender == GUI->SubtractFile_Edit )
   {
      m_instance.p_operandImageFilePath = text;
      UpdateSubtractSection();
   }
   else if ( sender == GUI->InputHints_Edit )
      m_instance.p_inputHints = text;
   else if ( sender == GUI->OutputHints_Edit )
      m_instance.p_outputHints = text;

   sender.SetText( text );
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::e_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->OutputDir_SelectButton )
      SelectDir();
   else if ( sender == GUI->PSFFits_CheckBox )
      m_instance.p_fitPSF = checked;
   else if ( sender == GUI->SubtractFile_SelectButton )
      SelectSubtractFile();
   else if ( sender == GUI->SubtractFile_ShowButton )
      ShowSubtractFile();
   else if ( sender == GUI->SubtractFile_ClearButton )
   {
      GUI->SubtractFile_Edit.SetText( m_instance.p_operandImageFilePath = TheCAOperandImageFilePathParameter->DefaultValue() );
      UpdateSubtractSection();
   }
   else if ( sender == GUI->Overwrite_CheckBox )
      m_instance.p_overwriteExistingFiles = checked;
   else if ( sender == GUI->CometPathMap_CheckBox )
      m_instance.p_generateCometPathMap = checked;
   else if ( sender == GUI->SubtractStars_RadioButton )
      m_instance.p_operandSubtractAligned = !checked;
   else if ( sender == GUI->SubtractComet_RadioButton )
      m_instance.p_operandSubtractAligned = checked;
   else if ( sender == GUI->LinearFit_CheckBox )
      m_instance.p_operandLinearFit = checked;
   else if ( sender == GUI->Normalize_CheckBox )
      m_instance.p_operandNormalize = checked;
   else if ( sender == GUI->DrzSaveSA_CheckBox )
      m_instance.p_drizzleWriteStarAlignedImage = checked;
   else if ( sender == GUI->DrzSaveCA_CheckBox )
      m_instance.p_drizzleWriteCometAlignedImage = checked;
   else if ( sender == GUI->ShowFirstFrame_PushButton )
   {
      if ( !m_instance.p_targets.IsEmpty() )
         OpenAndActivateImageWindow( m_instance.p_targets[0].path );
   }
   else if ( sender == GUI->ShowLastFrame_PushButton )
   {
      if ( !m_instance.p_targets.IsEmpty() )
         OpenAndActivateImageWindow( ( *m_instance.p_targets.ReverseBegin() ).path );
   }
   else if ( sender == GUI->ResetPositions_PushButton )
   {
      ResetFramePositions();
   }
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::e_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->X0_NumericEdit )
   {
      if ( !m_instance.p_targets.IsEmpty() )
         SetFramePosition( 0, DPoint( value, m_instance.p_targets[0].position.y ) );
   }
   else if ( sender == GUI->Y0_NumericEdit )
   {
      if ( !m_instance.p_targets.IsEmpty() )
         SetFramePosition( 0, DPoint( m_instance.p_targets[0].position.x, value ) );
   }
   else if ( sender == GUI->X1_NumericEdit )
   {
      if ( !m_instance.p_targets.IsEmpty() )
      {
         size_type index = m_instance.p_targets.UpperBound();
         SetFramePosition( index, DPoint( value, m_instance.p_targets[index].position.y ) );
      }
   }
   else if ( sender == GUI->Y1_NumericEdit )
   {
      if ( !m_instance.p_targets.IsEmpty() )
      {
         size_type index = m_instance.p_targets.UpperBound();
         SetFramePosition( index, DPoint( m_instance.p_targets[index].position.x, value ) );
      }
   }
   else if ( sender == GUI->RejectLow_NumericControl )
   {
      m_instance.p_operandLinearFitLow = value;
      if ( m_instance.p_operandLinearFitLow >= m_instance.p_operandLinearFitHigh )
      {
         m_instance.p_operandLinearFitHigh = m_instance.p_operandLinearFitLow + 0.05;
         if ( m_instance.p_operandLinearFitHigh > 1 )
         {
            m_instance.p_operandLinearFitLow = 0.95;
            m_instance.p_operandLinearFitHigh = 1;
         }
         UpdateControls();
      }
   }
   else if ( sender == GUI->RejectHigh_NumericControl )
   {
      m_instance.p_operandLinearFitHigh = value;
      if ( m_instance.p_operandLinearFitHigh <= m_instance.p_operandLinearFitLow )
      {
         m_instance.p_operandLinearFitLow = m_instance.p_operandLinearFitHigh - 0.05;
         if ( m_instance.p_operandLinearFitLow < 0 )
         {
            m_instance.p_operandLinearFitLow = 0;
            m_instance.p_operandLinearFitHigh = 0.05;
         }
         UpdateControls();
      }
   }
   else if ( sender == GUI->ClampingThreshold_NumericControl )
      m_instance.p_linearClampingThreshold = value;
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->PixelInterpolation_ComboBox )
   {
      m_instance.p_pixelInterpolation = itemIndex;
      GUI->ClampingThreshold_NumericControl.Enable(
         m_instance.p_pixelInterpolation == CAPixelInterpolation::BicubicSpline || m_instance.p_pixelInterpolation == CAPixelInterpolation::Lanczos3 || m_instance.p_pixelInterpolation == CAPixelInterpolation::Lanczos4 || m_instance.p_pixelInterpolation == CAPixelInterpolation::Lanczos5 );
   }
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->TargetImages_TreeBox.Viewport() )
      wantsFiles = true;
   else if ( sender == GUI->OutputDir_Edit )
      wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
   else if ( sender == GUI->SubtractFile_Edit )
      wantsFiles = files.Length() == 1 && File::Exists( files[0] );
}

// ----------------------------------------------------------------------------

void CometAlignmentInterface::e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->TargetImages_TreeBox.Viewport() )
   {
      StringList imageFiles;
      StringList drizzleFiles;
      bool recursive = IsControlOrCmdPressed();
      for ( const String& item : files )
      {
         if ( File::Exists( item ) )
         {
            String ext = File::ExtractSuffix( item ).CaseFolded();
            if ( ext == ".xdrz" || ext == ".drz" )
               drizzleFiles << item;
            else
               imageFiles << item;
         }
         else if ( File::DirectoryExists( item ) )
         {
            imageFiles << FileFormat::SupportedImageFiles( item, true/*toRead*/, false/*toWrite*/, recursive );
            drizzleFiles << FileFormat::DrizzleFiles( item, recursive );
         }

      }

      AddFiles( imageFiles );

      for ( const String& filePath : drizzleFiles )
      {
         String targetName = DrizzleTargetName( filePath );
         for ( CometAlignmentInstance::ImageItem& item : m_instance.p_targets )
            if ( File::ExtractName( item.path ) == targetName )
            {
               item.drzPath = filePath;
               break;
            }
      }

      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->OutputDir_Edit )
   {
      if ( File::DirectoryExists( files[0] ) )
         GUI->OutputDir_Edit.SetText( m_instance.p_outputDirectory = files[0] );
   }
   else if ( sender == GUI->SubtractFile_Edit )
   {
      if ( File::Exists( files[0] ) )
      {
         GUI->SubtractFile_Edit.SetText( m_instance.p_operandImageFilePath = files[0] );
         UpdateSubtractSection();
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CometAlignmentInterface::GUIData::GUIData( CometAlignmentInterface& w )
{
   pcl::Font fnt = w.Font();
   int xFixLabelWidth = fnt.Width( String( 'M', 5 ) );
   int xyLabelWidth = fnt.Width( String( 'X', 2 ) );
   int labelWidth1 = fnt.Width( String( "Clamping threshold:" "X" ) );
   int groupWidth1 = fnt.Width( String( "DrizzleIntegration" "XXXXXX" ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );
   const char* fixedWidthStyle1 = "QWidget {min-width: 8em; max-width: 8em;}";

   //

   Monitor_TreeBox.SetNumberOfColumns( 5 );
   Monitor_TreeBox.SetHeaderText( 0, "CPU#" );
   Monitor_TreeBox.SetHeaderText( 1, "File" );
   Monitor_TreeBox.SetHeaderText( 2, "Status" );
   Monitor_TreeBox.SetHeaderText( 3, "Row" );
   Monitor_TreeBox.Hide();

   //

   TargetImages_TreeBox.SetMinHeight( IMAGELIST_MINHEIGHT( fnt ) );
   TargetImages_TreeBox.SetNumberOfColumns( 11 ); // 10 visible plus 1 hidden for table stretching
   TargetImages_TreeBox.SetHeaderText( 0, "#" );
   TargetImages_TreeBox.SetHeaderAlignment( 0, TextAlign::Center );
   TargetImages_TreeBox.SetHeaderText( 1, "?" );
   TargetImages_TreeBox.SetHeaderAlignment( 1, TextAlign::Center );
   TargetImages_TreeBox.SetHeaderText( 2, "F" );
   TargetImages_TreeBox.SetHeaderAlignment( 2, TextAlign::Center );
   TargetImages_TreeBox.SetHeaderText( 3, "File" );
   TargetImages_TreeBox.SetHeaderAlignment( 3, TextAlign::Left|TextAlign::VertCenter );
   TargetImages_TreeBox.SetHeaderText( 4, "Observation Time" );
   TargetImages_TreeBox.SetHeaderAlignment( 4, TextAlign::Center );
   TargetImages_TreeBox.SetHeaderText( 5, "X" );
   TargetImages_TreeBox.SetHeaderAlignment( 5, TextAlign::Center );
   TargetImages_TreeBox.SetHeaderText( 6, "Y" );
   TargetImages_TreeBox.SetHeaderAlignment( 6, TextAlign::Center );
   TargetImages_TreeBox.SetHeaderText( 7, u"\u0394T" );
   TargetImages_TreeBox.SetHeaderAlignment( 7, TextAlign::Center );
   TargetImages_TreeBox.SetHeaderText( 8, u"\u0394X" );
   TargetImages_TreeBox.SetHeaderAlignment( 8, TextAlign::Center );
   TargetImages_TreeBox.SetHeaderText( 9, u"\u0394Y" );
   TargetImages_TreeBox.SetHeaderAlignment( 9, TextAlign::Center );
   TargetImages_TreeBox.SetHeaderText( 10, String() );

   TargetImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&CometAlignmentInterface::e_FileDrag, w );
   TargetImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&CometAlignmentInterface::e_FileDrop, w );

   TargetImages_TreeBox.EnableMultipleSelections();
   TargetImages_TreeBox.DisableRootDecoration();
   TargetImages_TreeBox.EnableAlternateRowColor();
   TargetImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&CometAlignmentInterface::e_CurrentNodeUpdated, w );
   TargetImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&CometAlignmentInterface::e_NodeActivated, w );
   TargetImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&CometAlignmentInterface::e_NodeSelectionUpdated, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of target frames.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_TargetFrames_ButtonClick, w );

   AddDrizzleFiles_PushButton.SetText( "Add Drizzle Files" );
   AddDrizzleFiles_PushButton.SetToolTip( "<p>Associate existing drizzle data files with input images.</p>"
      "<p>Drizzle data files carry the .xdrz suffix. Normally you should select .xdrz files generated by "
      "the StarAlignment process for the same files that you are using with this tool.</p>" );
   AddDrizzleFiles_PushButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_TargetFrames_ButtonClick, w );

   ClearDrizzleFiles_PushButton.SetText( "Clear Drizzle Files" );
   ClearDrizzleFiles_PushButton.SetToolTip( "<p>Remove all drizzle data files currently associated with input images.</p>"
      "<p>This removes just file associations, not the actual drizzle data files.</p>" );
   ClearDrizzleFiles_PushButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_TargetFrames_ButtonClick, w );

   SetReference_PushButton.SetText( "Set Reference" );
   SetReference_PushButton.SetToolTip( "<p>Make the currently selected file on the list the reference image.</p>"
      "<p>All images will aligned to the reference image.</p>" );
   SetReference_PushButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_TargetFrames_ButtonClick, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all target frames.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_TargetFrames_ButtonClick, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of target frames.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_TargetFrames_ButtonClick, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected target frames.</p>"
      "<p>Disabled target frames will be ignored during the CometAlignment process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_TargetFrames_ButtonClick, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected target frames.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_TargetFrames_ButtonClick, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of target frames.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_TargetFrames_ButtonClick, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for target frame files.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_TargetFrames_ButtonClick, w );

   TargetImages_Control.SetSizer( TargetImages_Sizer );

   TargetImages_SectionBar.SetTitle( "Target Frames" );
   TargetImages_SectionBar.SetSection( TargetImages_Control );
   TargetImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&CometAlignmentInterface::e_ToggleSection, w );

   TargetButtons_Sizer.SetSpacing( 4 );
   TargetButtons_Sizer.Add( AddFiles_PushButton );
   TargetButtons_Sizer.Add( SelectAll_PushButton );
   TargetButtons_Sizer.Add( AddDrizzleFiles_PushButton );
   TargetButtons_Sizer.Add( ClearDrizzleFiles_PushButton );
   TargetButtons_Sizer.Add( SetReference_PushButton );
   TargetButtons_Sizer.Add( InvertSelection_PushButton );
   TargetButtons_Sizer.Add( ToggleSelected_PushButton );
   TargetButtons_Sizer.Add( RemoveSelected_PushButton );
   TargetButtons_Sizer.Add( Clear_PushButton );
   TargetButtons_Sizer.Add( FullPaths_CheckBox );
   TargetButtons_Sizer.AddStretch();

   TargetImages_Sizer.SetSpacing( 4 );
   TargetImages_Sizer.Add( TargetImages_TreeBox, 100 );
   TargetImages_Sizer.Add( TargetButtons_Sizer );

   //

   FormatHints_SectionBar.SetTitle( "Format Hints" );
   FormatHints_SectionBar.SetSection( FormatHints_Control );
   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&CometAlignmentInterface::e_ToggleSection, w );

   const char* hintsToolTip = "<p><i>Format hints</i> allow you to override global file format settings for image "
      "files used by specific processes. In CometAlignment, input hints change the way input images of some particular "
      "file formats are loaded, while output hints modify the way output registered image files are written.</p>"
      "<p>For example, you can use the \"lower-range\" and \"upper-range\" input hints to load floating point FITS and "
      "TIFF files generated by other applications that don't use PixInsight's normalized [0,1] range. Similarly, you "
      "can specify the \"up-bottom\" output hint to write all registered images (in FITS format) with the coordinate "
      "origin at the top-left corner. Most standard file format modules support hints; each format supports a number of "
      "input and/or output hints that you can use for different purposes with tools that give you access to format hints.</p>";

   InputHints_Label.SetText( "Input hints:" );
   InputHints_Label.SetFixedWidth( labelWidth1 );
   InputHints_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   InputHints_Label.SetToolTip( hintsToolTip );

   InputHints_Edit.SetToolTip( hintsToolTip );
   InputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&CometAlignmentInterface::e_EditCompleted, w );

   InputHints_Sizer.SetSpacing( 4 );
   InputHints_Sizer.Add( InputHints_Label );
   InputHints_Sizer.Add( InputHints_Edit, 100 );

   OutputHints_Label.SetText( "Output hints:" );
   OutputHints_Label.SetFixedWidth( labelWidth1 );
   OutputHints_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   OutputHints_Label.SetToolTip( hintsToolTip );

   OutputHints_Edit.SetToolTip( hintsToolTip );
   OutputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&CometAlignmentInterface::e_EditCompleted, w );

   OutputHints_Sizer.SetSpacing( 4 );
   OutputHints_Sizer.Add( OutputHints_Label );
   OutputHints_Sizer.Add( OutputHints_Edit, 100 );

   FormatHints_Sizer.SetSpacing( 4 );
   FormatHints_Sizer.Add( InputHints_Sizer );
   FormatHints_Sizer.Add( OutputHints_Sizer );

   FormatHints_Control.SetSizer( FormatHints_Sizer );

   //

   const char* ToolTipOutputDir = "<p>This is the directory (or folder) where all output files will be written.</p>"
      "<p>If this field is left blank, output files will be written to the same directories as their "
      "corresponding target files. In this case, make sure that source directories are writable, or the "
      "CometAlignment process will fail.</p>";

   OutputDir_Label.SetText( "Output directory:" );
   OutputDir_Label.SetFixedWidth( labelWidth1 );
   OutputDir_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   OutputDir_Label.SetToolTip( ToolTipOutputDir );

   OutputDir_Edit.SetToolTip( ToolTipOutputDir );
   OutputDir_Edit.OnEditCompleted( (Edit::edit_event_handler)&CometAlignmentInterface::e_EditCompleted, w );
   OutputDir_Edit.OnFileDrag( (Control::file_drag_event_handler)&CometAlignmentInterface::e_FileDrag, w );
   OutputDir_Edit.OnFileDrop( (Control::file_drop_event_handler)&CometAlignmentInterface::e_FileDrop, w );

   OutputDir_SelectButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select-file.png" ) ) );
   OutputDir_SelectButton.SetScaledFixedSize( 22, 22 );
   OutputDir_SelectButton.SetToolTip( "<p>Select output directory</p>" );
   OutputDir_SelectButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );

   OutputDir_Sizer.SetSpacing( 4 );
   OutputDir_Sizer.Add( OutputDir_Label );
   OutputDir_Sizer.Add( OutputDir_Edit, 100 );
   OutputDir_Sizer.Add( OutputDir_SelectButton );

   const char* ToolTipPrefix =
      "<p>This is a prefix that will be appended to the file name of each registered image.</p>";
   Prefix_Label.SetText( "Prefix:" );
   Prefix_Label.SetFixedWidth( labelWidth1 );
   Prefix_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   Prefix_Label.SetToolTip( ToolTipPrefix );
   Prefix_Edit.SetFixedWidth( xFixLabelWidth );
   Prefix_Edit.SetToolTip( ToolTipPrefix );
   Prefix_Edit.OnEditCompleted( (Edit::edit_event_handler)&CometAlignmentInterface::e_EditCompleted, w );

   const char* ToolTipPostfix =
      "<p>This is a postfix that will be appended to the file name of each registered image.</p>";
   Postfix_Label.SetText( "Postfix:" );
   Postfix_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   Postfix_Label.SetToolTip( ToolTipPostfix );
   Postfix_Edit.SetFixedWidth( xFixLabelWidth );
   Postfix_Edit.SetToolTip( ToolTipPostfix );
   Postfix_Edit.OnEditCompleted( (Edit::edit_event_handler)&CometAlignmentInterface::e_EditCompleted, w );

   Overwrite_CheckBox.SetText( "Overwrite" );
   Overwrite_CheckBox.SetToolTip( "<p>If this option is selected, CometAlignment will overwrite "
      "existing files with the same names as generated output files. This can be dangerous because the original "
      "contents of overwritten files will be lost.</p>"
      "<p><b>** Warning: Enable this option <u>at your own risk.</u></b></p>" );
   Overwrite_CheckBox.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );

   //

   OutputChunks_Sizer.Add( Prefix_Label );
   OutputChunks_Sizer.AddSpacing( 4 );
   OutputChunks_Sizer.Add( Prefix_Edit );
   OutputChunks_Sizer.AddSpacing( 12 );
   OutputChunks_Sizer.Add( Postfix_Label );
   OutputChunks_Sizer.AddSpacing( 4 );
   OutputChunks_Sizer.Add( Postfix_Edit );
   OutputChunks_Sizer.AddSpacing( 12 );
   OutputChunks_Sizer.Add( Overwrite_CheckBox );
   OutputChunks_Sizer.AddStretch();

   //

   CometPathMap_CheckBox.SetText( "Generate comet path mask" );
   CometPathMap_CheckBox.SetToolTip( "<p>Generate a new image window with a representation of the comet's path "
      "from positions measured on the set of input frames. This image is generated with a white background and "
      "black center marks and lines, so it is ready to be used as a mask on any of the input frames.</p>" );
   CometPathMap_CheckBox.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );

   //

   CometPathMap_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   CometPathMap_Sizer.Add( CometPathMap_CheckBox );
   CometPathMap_Sizer.AddStretch();

   //

   Output_Sizer.SetSpacing( 4 );
   Output_Sizer.Add( OutputDir_Sizer );
   Output_Sizer.Add( OutputChunks_Sizer );
   Output_Sizer.Add( CometPathMap_Sizer );

   Output_Control.SetSizer( Output_Sizer );
   Output_SectionBar.SetTitle( "Output" );
   Output_SectionBar.SetSection( Output_Control );
   Output_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&CometAlignmentInterface::e_ToggleSection, w );

   //

   Parameter_Control.SetSizer( Parameter_Sizer );
   Parameter_SectionBar.SetTitle( "Parameters" );
   Parameter_SectionBar.SetSection( Parameter_Control );
   Parameter_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&CometAlignmentInterface::e_ToggleSection, w );

   //

   X0_NumericEdit.label.SetText( u"X\u2080" );
   X0_NumericEdit.label.SetMinWidth( xyLabelWidth );
   X0_NumericEdit.label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );
   X0_NumericEdit.SetPrecision( TheCATargetFrameXParameter->Precision() );
   X0_NumericEdit.EnableFixedPrecision();
   X0_NumericEdit.SetRange( TheCATargetFrameXParameter->MinimumValue(), TheCATargetFrameXParameter->MaximumValue() );
   X0_NumericEdit.edit.SetStyleSheet( fixedWidthStyle1 );
   X0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CometAlignmentInterface::e_ValueUpdated, w );
   X0_NumericEdit.SetToolTip( "<p>Nucleus X-coordinate, first image.</p>" );

   Y0_NumericEdit.label.SetText( u"Y\u2080" );
   Y0_NumericEdit.label.SetMinWidth( xyLabelWidth );
   Y0_NumericEdit.label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );
   Y0_NumericEdit.SetPrecision( TheCATargetFrameYParameter->Precision() );
   Y0_NumericEdit.EnableFixedPrecision();
   Y0_NumericEdit.SetRange( TheCATargetFrameYParameter->MinimumValue(), TheCATargetFrameYParameter->MaximumValue() );
   Y0_NumericEdit.edit.SetStyleSheet( fixedWidthStyle1 );
   Y0_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CometAlignmentInterface::e_ValueUpdated, w );
   Y0_NumericEdit.SetToolTip( "<p>Nucleus Y-coordinate, first image.</p>" );

   X1_NumericEdit.label.SetText( u"X\u2081" );
   X1_NumericEdit.label.SetMinWidth( xyLabelWidth );
   X1_NumericEdit.label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );
   X1_NumericEdit.SetPrecision( TheCATargetFrameXParameter->Precision() );
   X1_NumericEdit.EnableFixedPrecision();
   X1_NumericEdit.SetRange( TheCATargetFrameXParameter->MinimumValue(), TheCATargetFrameXParameter->MaximumValue() );
   X1_NumericEdit.edit.SetStyleSheet( fixedWidthStyle1 );
   X1_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CometAlignmentInterface::e_ValueUpdated, w );
   X1_NumericEdit.SetToolTip( "<p>Nucleus X-coordinate, last image.</p>" );

   Y1_NumericEdit.label.SetText( u"Y\u2081" );
   Y1_NumericEdit.label.SetMinWidth( xyLabelWidth );
   Y1_NumericEdit.label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );
   Y1_NumericEdit.SetPrecision( TheCATargetFrameYParameter->Precision() );
   Y1_NumericEdit.EnableFixedPrecision();
   Y1_NumericEdit.SetRange( TheCATargetFrameYParameter->MinimumValue(), TheCATargetFrameYParameter->MaximumValue() );
   Y1_NumericEdit.edit.SetStyleSheet( fixedWidthStyle1 );
   Y1_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CometAlignmentInterface::e_ValueUpdated, w );
   Y1_NumericEdit.SetToolTip( "<p>Nucleus Y-coordinate, last image.</p>" );

   //

   XDot_NumericEdit.label.SetText( u"\u1E8A" ); // Latin Capital Letter X with Dot Above
   XDot_NumericEdit.label.SetMinWidth( xyLabelWidth );
   XDot_NumericEdit.label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );
   XDot_NumericEdit.SetPrecision( TheCATargetFrameXParameter->Precision() );
   XDot_NumericEdit.EnableFixedPrecision();
   XDot_NumericEdit.SetRange( int32_min, int32_max );
   XDot_NumericEdit.edit.SetReadOnly();
   XDot_NumericEdit.edit.SetStyleSheet( fixedWidthStyle1 );
   XDot_NumericEdit.SetToolTip( "<p>Apparent mean comet velocity, X component (pixels per hour).</p>" );

   YDot_NumericEdit.label.SetText( u"\u1E8E" ); // Latin Capital Letter Y with Dot Above
   YDot_NumericEdit.label.SetMinWidth( xyLabelWidth );
   YDot_NumericEdit.label.SetTextAlignment( TextAlign::Left | TextAlign::VertCenter );
   YDot_NumericEdit.SetPrecision( TheCATargetFrameYParameter->Precision() );
   YDot_NumericEdit.EnableFixedPrecision();
   YDot_NumericEdit.SetRange( int32_min, int32_max );
   YDot_NumericEdit.edit.SetReadOnly();
   YDot_NumericEdit.edit.SetStyleSheet( fixedWidthStyle1 );
   YDot_NumericEdit.SetToolTip( "<p>Apparent mean comet velocity, Y component (pixels per hour).</p>" );

   //

   ShowFirstFrame_PushButton.SetText( "Show First Image" );
   ShowFirstFrame_PushButton.SetStyleSheet( fixedWidthStyle1 );
   ShowFirstFrame_PushButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );
   ShowFirstFrame_PushButton.SetToolTip( "<p>Open or bring the first image to front.</p>"
      "<p>Click on the comet to acquire centroid coordinates of the nucleus. Use "
#ifdef __PCL_MACOSX
      "Cmd"
#else
      "Ctrl"
#endif
      "+Click to disable automatic centroid detection.</p>" );

   ShowLastFrame_PushButton.SetText( "Show Last Image" );
   ShowLastFrame_PushButton.SetStyleSheet( fixedWidthStyle1 );
   ShowLastFrame_PushButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );
   ShowLastFrame_PushButton.SetToolTip( "<p>Open or bring the last image to front.</p>"
      "<p>Click on the comet to acquire centroid coordinates of the nucleus. Use "
#ifdef __PCL_MACOSX
      "Cmd"
#else
      "Ctrl"
#endif
      "+Click to disable automatic centroid detection.</p>" );

   ResetPositions_PushButton.SetText( "Reset Positions" );
   ResetPositions_PushButton.SetStyleSheet( fixedWidthStyle1 );
   ResetPositions_PushButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );
   ResetPositions_PushButton.SetToolTip( "<p>Clear all frame positions.</p>"
      "<p>You'll have to redefine the first and last frame positions, as well as all required intermediate "
      "frame positions, after this action.</p>" );

   //

   PSFFits_CheckBox.SetText( "Compute PSF fits" );
   PSFFits_CheckBox.SetToolTip( "<p>Compute comet nucleus centroids by fitting a PSF for each target frame "
      "during process execution. This option should normally be enabled for optimal registration accuracy.</p>"
      "<p>If this option is disabled, comet coordinates will be interpolated linearly from fixed frame positions, "
      "or only from the initial and final frame positions if no intermediate frame positions have been defined.</p>" );
   PSFFits_CheckBox.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );

   //

   PSFFits_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   PSFFits_Sizer.Add( PSFFits_CheckBox );
   PSFFits_Sizer.AddStretch();

   //

   FirstFrame_Sizer.AddUnscaledSpacing( labelWidth1 - xyLabelWidth );
   FirstFrame_Sizer.Add( X0_NumericEdit );
   FirstFrame_Sizer.AddSpacing( 8 );
   FirstFrame_Sizer.Add( Y0_NumericEdit );
   FirstFrame_Sizer.AddSpacing( 8 );
   FirstFrame_Sizer.Add( ShowFirstFrame_PushButton );
   FirstFrame_Sizer.AddStretch();

   LastFrame_Sizer.AddUnscaledSpacing( labelWidth1 - xyLabelWidth );
   LastFrame_Sizer.Add( X1_NumericEdit );
   LastFrame_Sizer.AddSpacing( 8 );
   LastFrame_Sizer.Add( Y1_NumericEdit );
   LastFrame_Sizer.AddSpacing( 8 );
   LastFrame_Sizer.Add( ShowLastFrame_PushButton );
   LastFrame_Sizer.AddStretch();

   Velocity_Sizer.AddUnscaledSpacing( labelWidth1 - xyLabelWidth );
   Velocity_Sizer.Add( XDot_NumericEdit );
   Velocity_Sizer.AddSpacing( 8 );
   Velocity_Sizer.Add( YDot_NumericEdit );
   Velocity_Sizer.AddSpacing( 8 );
   Velocity_Sizer.Add( ResetPositions_PushButton );
   Velocity_Sizer.AddStretch();

   Parameter_Sizer.SetSpacing( 4 );
   Parameter_Sizer.Add( FirstFrame_Sizer );
   Parameter_Sizer.Add( LastFrame_Sizer );
   Parameter_Sizer.Add( Velocity_Sizer );
   Parameter_Sizer.Add( PSFFits_Sizer );

   //

   const char* ToolTipSubtract = "<p>The selected image will be subtracted from each target image.</p>"
      "<p>With <i>Comet aligned</i> checked: The selected operand image (usually a pure comet image) "
      "will be aligned to the comet's position in each target frame and subtracted from it.</p>"
      "<p>With <i>Stars aligned</i> checked: The selected operand image (usually a pure star field) "
      "will be subtracted from each target frame and the result will be the comet aligned to the comet's "
      "position in the reference frame.</p>"
      "<p>Note: When drizzle data files (.xdrz) are added, the module uses the original calibrated frames "
      "instead of the registered images for all manipulations. In this way we can create a comet-aligned "
      "image directly from unregistered originals to avoid a double registration/interpolation. Also, if "
      "drizzle data files are added and an operand image is selected for subtraction, the module will write "
      "new unregistered original frames suitable for drizzle integration.</p>";

   Subtract_Control.SetSizer( Subtract_Sizer );
   Subtract_SectionBar.SetTitle( "Subtract" );
   Subtract_SectionBar.SetSection( Subtract_Control );
   Subtract_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&CometAlignmentInterface::e_ToggleSection, w );

   SubtractFile_Label.SetText( "Operand image:" );
   SubtractFile_Label.SetFixedWidth( labelWidth1 );
   SubtractFile_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   SubtractFile_Label.SetToolTip( ToolTipSubtract );

   SubtractFile_Edit.SetToolTip( ToolTipSubtract );
   SubtractFile_Edit.OnEditCompleted( (Edit::edit_event_handler)&CometAlignmentInterface::e_EditCompleted, w );
   SubtractFile_Edit.OnFileDrag( (Control::file_drag_event_handler)&CometAlignmentInterface::e_FileDrag, w );
   SubtractFile_Edit.OnFileDrop( (Control::file_drop_event_handler)&CometAlignmentInterface::e_FileDrop, w );

   SubtractFile_SelectButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select-file.png" ) ) );
   SubtractFile_SelectButton.SetScaledFixedSize( 22, 22 );
   SubtractFile_SelectButton.SetToolTip( "<p>Select the operand image file.</p>" );
   SubtractFile_SelectButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );

   SubtractFile_ShowButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/window-new.png" ) ) );
   SubtractFile_ShowButton.SetScaledFixedSize( 22, 22 );
   SubtractFile_ShowButton.SetToolTip( "<p>Open the operand image file.</p>" );
   SubtractFile_ShowButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );

   SubtractFile_ClearButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/clear.png" ) ) );
   SubtractFile_ClearButton.SetScaledFixedSize( 22, 22 );
   SubtractFile_ClearButton.SetToolTip( "<p>Clear operand image file.</p>" );
   SubtractFile_ClearButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );

   SubtractFile_Sizer.SetSpacing( 4 );
   SubtractFile_Sizer.Add( SubtractFile_Label );
   SubtractFile_Sizer.Add( SubtractFile_Edit, 100 );
   SubtractFile_Sizer.Add( SubtractFile_SelectButton );
   SubtractFile_Sizer.Add( SubtractFile_ShowButton );
   SubtractFile_Sizer.Add( SubtractFile_ClearButton );

   //

   SubtractStars_RadioButton.SetText( "Stars aligned" );
   SubtractStars_RadioButton.SetToolTip( "<p>The Operand image is the star field aligned to the reference image.</p>" );
   SubtractStars_RadioButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );

   SubtractComet_RadioButton.SetText( "Comet aligned" );
   SubtractComet_RadioButton.SetToolTip( "<p>The Operand image contains the comet aligned to the reference image.</p>" );
   SubtractComet_RadioButton.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );

   SubtractImgType_Sizer.SetMargin( 6 );
   SubtractImgType_Sizer.SetSpacing( 4 );
   SubtractImgType_Sizer.Add( SubtractStars_RadioButton );
   SubtractImgType_Sizer.Add( SubtractComet_RadioButton );

   SubtractImgType_GroupBox.SetTitle( "Operand Type" );
   SubtractImgType_GroupBox.SetToolTip( ToolTipSubtract );
   SubtractImgType_GroupBox.SetSizer( SubtractImgType_Sizer );
   SubtractImgType_GroupBox.SetMinWidth( groupWidth1 );

   //

   const char* toolTipDrzSave = "<p>When drizzle data files (.xdrz) are added and a subtraction operand image is selected, "
      "a new, unaligned drizzle-integrable image will be written (either a stars-only or a comet-only image), "
      "along with new .xdrz files.</p>";

   DrzSaveSA_CheckBox.SetText( "Stars aligned" );
   DrzSaveSA_CheckBox.SetToolTip( toolTipDrzSave );
   DrzSaveSA_CheckBox.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );

   DrzSaveCA_CheckBox.SetText( "Comet aligned" );
   DrzSaveCA_CheckBox.SetToolTip( toolTipDrzSave );
   DrzSaveCA_CheckBox.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );

   DrzSave_Sizer.SetMargin( 6 );
   DrzSave_Sizer.SetSpacing( 4 );
   DrzSave_Sizer.Add( DrzSaveSA_CheckBox );
   DrzSave_Sizer.Add( DrzSaveCA_CheckBox );

   DrzSave_GroupBox.SetTitle( "Drizzle Output" );
   DrzSave_GroupBox.SetToolTip( toolTipDrzSave );
   DrzSave_GroupBox.SetSizer( DrzSave_Sizer );
   DrzSave_GroupBox.SetMinWidth( groupWidth1 );

   SubtractImgOption_Sizer.SetSpacing( 4 );
   SubtractImgOption_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SubtractImgOption_Sizer.Add( SubtractImgType_GroupBox );
   SubtractImgOption_Sizer.Add( DrzSave_GroupBox );
   SubtractImgOption_Sizer.AddStretch();

   //

   LinearFit_CheckBox.SetText( "Linear fit" );
   LinearFit_CheckBox.SetToolTip( "<p>Apply a linear fit before operand image subtraction.</p>" );
   LinearFit_CheckBox.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );

   //

   Normalize_CheckBox.SetText( "Normalize" );
   Normalize_CheckBox.SetToolTip( "<p>Equalize median values after operand image subtraction.</p>" );
   Normalize_CheckBox.OnClick( (Button::click_event_handler)&CometAlignmentInterface::e_ButtonClick, w );

   //

   SubtractChekers_Sizer.SetSpacing( 20 );
   SubtractChekers_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SubtractChekers_Sizer.Add( LinearFit_CheckBox );
   SubtractChekers_Sizer.Add( Normalize_CheckBox );
   SubtractChekers_Sizer.AddStretch();

   //

   RejectLow_NumericControl.label.SetText( "Reject low:" );
   RejectLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   RejectLow_NumericControl.slider.SetRange( 0, 100 );
   RejectLow_NumericControl.slider.SetScaledMinWidth( 200 );
   RejectLow_NumericControl.SetReal();
   RejectLow_NumericControl.SetRange( TheCAOperandLinearFitLowParameter->MinimumValue(), TheCAOperandLinearFitLowParameter->MaximumValue() );
   RejectLow_NumericControl.SetPrecision( TheCAOperandLinearFitLowParameter->Precision() );
   RejectLow_NumericControl.SetToolTip( "<p>Low rejection limit. LinearFit will ignore all reference and target "
      "pixels whose values are less than or equal to this limit. Note that according to that definition, black "
      "pixels (zero pixel values) are always rejected.</p>" );
   RejectLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CometAlignmentInterface::e_ValueUpdated, w );

   //

   RejectHigh_NumericControl.label.SetText( "Reject high:" );
   RejectHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   RejectHigh_NumericControl.slider.SetRange( 0, 100 );
   RejectHigh_NumericControl.slider.SetScaledMinWidth( 200 );
   RejectHigh_NumericControl.SetReal();
   RejectHigh_NumericControl.SetRange( TheCAOperandLinearFitHighParameter->MinimumValue(), TheCAOperandLinearFitHighParameter->MaximumValue() );
   RejectHigh_NumericControl.SetPrecision( TheCAOperandLinearFitHighParameter->Precision() );
   RejectHigh_NumericControl.SetToolTip( "<p>High rejection limit. LinearFit will ignore all reference and target "
      "pixels whose values are greater than or equal to this limit. Note that according to that definition, white "
      "pixels (pixel values equal to one in the normalized [0,1] range) are always rejected.</p>" );
   RejectHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CometAlignmentInterface::e_ValueUpdated, w );

   //

   Subtract_Sizer.SetSpacing( 4 );
   Subtract_Sizer.Add( SubtractFile_Sizer );
   Subtract_Sizer.Add( SubtractImgOption_Sizer );
   Subtract_Sizer.Add( SubtractChekers_Sizer );
   Subtract_Sizer.Add( RejectLow_NumericControl );
   Subtract_Sizer.Add( RejectHigh_NumericControl );

   //---------------------------------------------------

   Interpolation_SectionBar.SetTitle( "Interpolation" );
   Interpolation_SectionBar.SetSection( Interpolation_Control );
   Interpolation_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&CometAlignmentInterface::e_ToggleSection, w );

   const char* pixelInterpolationToolTip = "<p>Pixel interpolation algorithm</p>"
      "<p><b>Lanczos</b> is, in general, the best interpolation algorithm for image registration of linear "
      "images when no large scale changes are required. Lanczos has excellent detail preservation and subpixel "
      "registration performance with minimal generation of aliasing effects. Its main drawback is generation of "
      "undershooting (ringing) artifacts, but this problem can be fixed in most cases with the implemented "
      "<i>clamping</i> mechanism (see the <i>clamping threshold</i> parameter below).</p>"
      "<p><b>Bicubic spline</b> is an excellent interpolation algorithm in terms of accuracy and execution "
      "speed. As Lanczos interpolation, bicubic spline generates ringing artifacts, but the implemented clamping "
      "mechanism can usually fix most of them. The main problem with bicubic spline is generation of aliasing "
      "effects, such as moire patterns, which can be problematic with noisy images, especially on sky background "
      "areas. This is one of the reasons why Lanczos is in general the preferred option.</p>"
      "<p><b>Bilinear interpolation</b> can be useful to register low SNR linear images, in the rare cases where "
      "Lanczos and bicubic spline interpolation generates too strong oscillations between noisy pixels that can't "
      "be avoided completely with the clamping feature. Bilinear interpolation has no ringing artifacts, but it "
      "generates strong aliasing patterns on noisy areas and provides less accurate results than Lanczos and "
      "bicubic spline.</p>"
      "<p><b>Cubic filter interpolations</b>, such as Mitchell-Netravali, Catmull-Rom spline and cubic B-spline, "
      "provide higher smoothness and subsampling accuracy that can be necessary when the registration transformation "
      "involves relatively strong size reductions.</p>"
      "<p><b>Nearest neighbor</b> is the simplest possible pixel interpolation method. It always produces the "
      "worst results, especially in terms of registration accuracy (no subpixel registration is possible), and "
      "discontinuities due to the simplistic interpolation scheme. However, in absence of scaling and rotation "
      "nearest neighbor preserves the original noise distribution in the registered images, a property that can "
      "be useful in some image analysis applications.</p>";

   PixelInterpolation_Label.SetText( "Pixel interpolation:" );
   PixelInterpolation_Label.SetFixedWidth( labelWidth1 );
   PixelInterpolation_Label.SetTextAlignment( TextAlign::Right | TextAlign::VertCenter );
   PixelInterpolation_Label.SetToolTip( pixelInterpolationToolTip );

   PixelInterpolation_ComboBox.AddItem( "Nearest Neighbor" );
   PixelInterpolation_ComboBox.AddItem( "Bilinear" );
   PixelInterpolation_ComboBox.AddItem( "Bicubic Spline" );
   PixelInterpolation_ComboBox.AddItem( "Bicubic B-Spline" );
   PixelInterpolation_ComboBox.AddItem( "Lanczos-3" );
   PixelInterpolation_ComboBox.AddItem( "Lanczos-4" );
   PixelInterpolation_ComboBox.AddItem( "Lanczos-5" );
   PixelInterpolation_ComboBox.AddItem( "Mitchell-Netravali Filter" );
   PixelInterpolation_ComboBox.AddItem( "Catmull-Rom Spline Filter" );
   PixelInterpolation_ComboBox.AddItem( "Cubic B-Spline Filter" );
   PixelInterpolation_ComboBox.SetMaxVisibleItemCount( 16 );
   PixelInterpolation_ComboBox.SetToolTip( pixelInterpolationToolTip );
   PixelInterpolation_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&CometAlignmentInterface::e_ItemSelected, w );

   PixelInterpolation_Sizer.SetSpacing( 4 );
   PixelInterpolation_Sizer.Add( PixelInterpolation_Label );
   PixelInterpolation_Sizer.Add( PixelInterpolation_ComboBox );
   PixelInterpolation_Sizer.AddStretch();

   ClampingThreshold_NumericControl.label.SetText( "Clamping threshold:" );
   ClampingThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   ClampingThreshold_NumericControl.slider.SetRange( 0, 100 );
   ClampingThreshold_NumericControl.slider.SetScaledMinWidth( 200 );
   ClampingThreshold_NumericControl.SetReal();
   ClampingThreshold_NumericControl.SetRange( TheCALinearClampingThresholdParameter->MinimumValue(), TheCALinearClampingThresholdParameter->MaximumValue() );
   ClampingThreshold_NumericControl.SetPrecision( TheCALinearClampingThresholdParameter->Precision() );
   ClampingThreshold_NumericControl.SetToolTip(
      "<p>Clamping threshold for the bicubic spline and Lanczos interpolation algorithms.</p>"
      "<p>The main drawback of the Lanczos and bicubic spline interpolation algorithms is generation of strong "
      "undershooting (ringing) artifacts. This is caused by negative lobes of the interpolation functions falling "
      "over high-contrast edges and isolated bright pixels. This usually happens with linear data, such as raw CCD "
      "and DSLR images, and rarely with nonlinear or stretched images.</p>"
      "<p>In the current PixInsight/PCL implementations of these algorithms we have included a <i>clamping</i> "
      "mechanism that prevents negative interpolated values and ringing artifacts for most images. This parameter "
      "represents a threshold value to trigger interpolation clamping. A lower threshold leads to a more aggressive "
      "deringing effect; however, too low of a clamping threshold can degrade interpolation performance, especially "
      "in terms of aliasing and detail preservation.</p>" );
   ClampingThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CometAlignmentInterface::e_ValueUpdated, w );

   Interpolation_Sizer.SetSpacing( 4 );
   Interpolation_Sizer.Add( PixelInterpolation_Sizer );
   Interpolation_Sizer.Add( ClampingThreshold_NumericControl );

   Interpolation_Control.SetSizer( Interpolation_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Monitor_TreeBox );
   Global_Sizer.Add( TargetImages_SectionBar );
   Global_Sizer.Add( TargetImages_Control );
   Global_Sizer.Add( FormatHints_SectionBar );
   Global_Sizer.Add( FormatHints_Control );
   Global_Sizer.Add( Output_SectionBar );
   Global_Sizer.Add( Output_Control );

   Global_Sizer.Add( Parameter_SectionBar );
   Global_Sizer.Add( Parameter_Control );

   Global_Sizer.Add( Subtract_SectionBar );
   Global_Sizer.Add( Subtract_Control );
   Global_Sizer.Add( Interpolation_SectionBar );
   Global_Sizer.Add( Interpolation_Control );

   w.SetSizer( Global_Sizer );

   FormatHints_Control.Hide();

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetMinWidth();
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF CometAlignmentInterface.cpp - Released 2024-12-23T11:33:28Z
