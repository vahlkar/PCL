//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.2
// ----------------------------------------------------------------------------
// DynamicPSFInterface.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "DynamicPSFInterface.h"
#include "DynamicPSFProcess.h"
#include "StarDetector.h"

#include <pcl/Dialog.h>
#include <pcl/FileDialog.h>
#include <pcl/GaussianFilter.h>
#include <pcl/GlobalSettings.h>
#include <pcl/GroupBox.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/MoffatFilter.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/Settings.h>
#include <pcl/TextBox.h>
#include <pcl/VariableShapeFilter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

DynamicPSFInterface* TheDynamicPSFInterface = nullptr;

static const pcl_bool* signedAngles = nullptr;

// Half size of center mark in logical viewport pixels.
#define CENTER_RADIUS   5

#define DATATREE_MINHEIGHT( fnt )   RoundInt( 12.125*fnt.Height() )

#define MIN_FONT_SIZE   4
#define MAX_FONT_SIZE   12

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class SortStarsDialog : public Dialog
{
public:

   DynamicPSFInterface::SortingCriterion sortBy = DynamicPSFInterface::SortByMAD;

   SortStarsDialog()
   {
      SortingCriterion_GroupBox.SetTitle( "Sorting Criterion" );

      SortByStarId_RadioButton.SetText(           "Star Id" );
      SortByBackground_RadioButton.SetText(       "Background" );
      SortByAmplitude_RadioButton.SetText(        "Amplitude" );
      SortBySigma_RadioButton.SetText(            "Standard deviation" );
      SortByAspectRatio_RadioButton.SetText(      "Aspect ratio" );
      SortByRotationAngle_RadioButton.SetText(    "Rotation angle" );
      SortByAbsRotationAngle_RadioButton.SetText( "Rotation angle (absolute value)" );
      SortByShape_RadioButton.SetText(            "Beta exponent" );
      SortByFlux_RadioButton.SetText(             "Total flux" );
      SortByMeanSignal_RadioButton.SetText(       "Mean signal" );
      SortByMAD_RadioButton.SetText(              "Mean absolute difference" );
      SortByMAD_RadioButton.SetChecked();

      SortingCriterion_Sizer.SetMargin( 6 );
      SortingCriterion_Sizer.SetSpacing( 6 );
      SortingCriterion_Sizer.Add( SortByStarId_RadioButton );
      SortingCriterion_Sizer.Add( SortByBackground_RadioButton );
      SortingCriterion_Sizer.Add( SortByAmplitude_RadioButton );
      SortingCriterion_Sizer.Add( SortBySigma_RadioButton );
      SortingCriterion_Sizer.Add( SortByAspectRatio_RadioButton );
      SortingCriterion_Sizer.Add( SortByRotationAngle_RadioButton );
      SortingCriterion_Sizer.Add( SortByAbsRotationAngle_RadioButton );
      SortingCriterion_Sizer.Add( SortByShape_RadioButton );
      SortingCriterion_Sizer.Add( SortByFlux_RadioButton );
      SortingCriterion_Sizer.Add( SortByMeanSignal_RadioButton );
      SortingCriterion_Sizer.Add( SortByMAD_RadioButton );

      SortingCriterion_GroupBox.SetSizer( SortingCriterion_Sizer );
      SortingCriterion_GroupBox.SetMinWidth( Font().Width( String( '0', 40 ) ) );

      OK_PushButton.SetText( "OK" );
      OK_PushButton.SetDefault();
      OK_PushButton.SetCursor( StdCursor::Checkmark );
      OK_PushButton.OnClick( (Button::click_event_handler)&SortStarsDialog::__Button_Click, *this );

      Cancel_PushButton.SetText( "Cancel" );
      Cancel_PushButton.SetCursor( StdCursor::Crossmark );
      Cancel_PushButton.OnClick( (Button::click_event_handler)&SortStarsDialog::__Button_Click, *this );

      Buttons_Sizer.SetSpacing( 8 );
      Buttons_Sizer.AddStretch();
      Buttons_Sizer.Add( OK_PushButton );
      Buttons_Sizer.Add( Cancel_PushButton );

      Global_Sizer.SetMargin( 8 );
      Global_Sizer.SetSpacing( 6 );
      Global_Sizer.Add( SortingCriterion_GroupBox );
      Global_Sizer.AddSpacing( 6 );
      Global_Sizer.Add( Buttons_Sizer );

      SetSizer( Global_Sizer );

      EnsureLayoutUpdated();
      AdjustToContents();
      SetFixedSize();

      SetWindowTitle( "Sort PSF Collections" );
   }

private:

   VerticalSizer  Global_Sizer;
      GroupBox       SortingCriterion_GroupBox;
      VerticalSizer  SortingCriterion_Sizer;
         RadioButton SortByStarId_RadioButton;
         RadioButton SortByBackground_RadioButton;
         RadioButton SortByAmplitude_RadioButton;
         RadioButton SortBySigma_RadioButton;
         RadioButton SortByAspectRatio_RadioButton;
         RadioButton SortByRotationAngle_RadioButton;
         RadioButton SortByAbsRotationAngle_RadioButton;
         RadioButton SortByShape_RadioButton;
         RadioButton SortByFlux_RadioButton;
         RadioButton SortByMeanSignal_RadioButton;
         RadioButton SortByMAD_RadioButton;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void __Button_Click( Button& sender, bool checked )
   {
      if ( sender == OK_PushButton )
      {
         if ( SortByStarId_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByStarId;
         if ( SortByBackground_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByBackground;
         if ( SortByAmplitude_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByAmplitude;
         if ( SortBySigma_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortBySigma;
         if ( SortByAspectRatio_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByAspectRatio;
         if ( SortByRotationAngle_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByRotationAngle;
         if ( SortByAbsRotationAngle_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByAbsRotationAngle;
         if ( SortByShape_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByShape;
         if ( SortByFlux_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByFlux;
         if ( SortByMeanSignal_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByMeanSignal;
         if ( SortByMAD_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByMAD;
         Ok();
      }
      else if ( sender == Cancel_PushButton )
         Cancel();
   }
};

static SortStarsDialog* s_sortStarsDialog = nullptr;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class AverageStarDialog : public Dialog
{
public:

   typedef DynamicPSFInterface::star_list star_list;
   typedef DynamicPSFInterface::psf_list  psf_list;

   AverageStarDialog( const star_list& stars, float xScale, float yScale )
   {
      Data_TextBox.SetReadOnly();
      Data_TextBox.SetMinSize( Font().Width( String( '0', 40 ) ), LogicalPixelsToPhysical( 250 ) );

      RenderPSF_ToolButton.SetIcon( ScaledResource( ":/icons/camera.png" ) );
      RenderPSF_ToolButton.SetScaledFixedSize( 24, 24 );
      RenderPSF_ToolButton.SetToolTip( "<p>Render the average PSF as a new image.</p>" );
      RenderPSF_ToolButton.OnClick( (Button::click_event_handler)&AverageStarDialog::__Button_Click, *this );

      OK_PushButton.SetText( "OK" );
      OK_PushButton.SetDefault();
      OK_PushButton.SetCursor( StdCursor::Checkmark );
      OK_PushButton.OnClick( (Button::click_event_handler)&AverageStarDialog::__Button_Click, *this );

      Buttons_Sizer.Add( RenderPSF_ToolButton );
      Buttons_Sizer.AddStretch();
      Buttons_Sizer.Add( OK_PushButton );

      Global_Sizer.SetMargin( 8 );
      Global_Sizer.SetSpacing( 6 );
      Global_Sizer.Add( Data_TextBox );
      Global_Sizer.AddSpacing( 6 );
      Global_Sizer.Add( Buttons_Sizer );

      SetSizer( Global_Sizer );

      EnsureLayoutUpdated();
      AdjustToContents();
      SetMinSize();

      SetWindowTitle( "Average Star Data" );

      N = 0;
      variableShape = -1;
      B = A = sx = sy = theta = beta = flux = meanSignal = mad = 0;
      for ( const DynamicPSFInterface::Star& star : stars )
         for ( const DynamicPSFInterface::PSF& psf : star.psfs )
            if ( psf )
            {
               bool thisVariableShape = psf.function == PSFunction::Gaussian || psf.function == PSFunction::VariableShape;
               if ( variableShape < 0 )
                  variableShape = thisVariableShape;
               else if ( variableShape != thisVariableShape )
                  throw Error( "Internal Error: Incongruent PSF functions in the set of selected stars." );
               B += psf.B;
               A += psf.A;
               sx += psf.sx;
               sy += psf.sy;
               theta += (*signedAngles && psf.theta > 90) ? psf.theta - 180 : psf.theta;
               beta += psf.beta;
               flux += psf.flux;
               meanSignal += psf.meanSignal;
               mad += psf.mad;
               ++N;
            }

      if ( N == 0 )
         throw Error( "Internal Error: Insufficient PSF data." );

      B /= N;
      A /= N;
      sx /= N;
      sy /= N;
      theta /= N;
      beta /= N;
      flux /= N;
      meanSignal /= N;
      mad /= N;

      bool isXScale = xScale > 0; if ( !isXScale ) xScale = 1;
      bool isYScale = yScale > 0; if ( !isYScale ) yScale = 1;
      PSFData::psf_function function = variableShape ? PSFunction::VariableShape : PSFunction::Moffat;

      IsoString fmt =
         "<monospace>"
         "Average %s PSF\n"
         "N ........ %d stars\n"
         "B ........ %10.6f\n"
         "A ........ %10.6f\n"
         "sx ....... %6.2f px\n"
         "sy ....... %6.2f px\n"
         "FWHMx .... %6.2f %s\n"
         "FWHMy .... %6.2f %s\n"
         "r ........ %7.3f\n"
         "theta .... %";
      if ( *signedAngles )
         fmt += '+';
      fmt +=
         "6.2f deg\n"
         "beta ..... %6.2f\n"
         "flux ..... %.3e\n"
         "signal ... %.3e\n"
         "MAD ...... %.3e\n";

      Data_TextBox.SetText( String().Format(
         fmt.c_str(),
         variableShape ? ((beta == 2) ? "Gaussian" : "VarShape") : "Moffat",
         N,
         B,
         A,
         sx,
         sy,
         PSFData::FWHM( function, sx, beta )*xScale, isXScale ? "\"" : "px",
         PSFData::FWHM( function, sy, beta )*yScale, isYScale ? "\"" : "px",
         sy/sx,
         theta,
         beta,
         flux,
         meanSignal,
         mad ) );
   }

private:

   int    N;
   int    variableShape;
   double B;
   double A;
   double sx;
   double sy;
   double theta;
   double beta;
   double flux;
   double meanSignal;
   double mad;

   VerticalSizer  Global_Sizer;
      TextBox     Data_TextBox;
      HorizontalSizer   Buttons_Sizer;
         ToolButton        RenderPSF_ToolButton;
         PushButton        OK_PushButton;

   void __Button_Click( Button& sender, bool checked )
   {
      if ( sender == RenderPSF_ToolButton )
      {
         ImageWindow w( 1, 1, 1, 32, true/*floatSample*/, false/*color*/, true/*initialProcessing*/, "PSF" );
         ImageVariant v = w.MainView().Image();

         if ( variableShape )
         {
            VariableShapeFilter V( float( sx ), beta, 0.01F/*eps*/, sy/sx/*rho*/, Rad( theta ) );
            V.ToImage( v );
         }
         else
         {
            MoffatFilter M( float( sx ), beta, 0.01F/*eps*/, sy/sx/*rho*/, Rad( theta ) );
            M.ToImage( v );
         }

         w.Show();
      }
      else if ( sender == OK_PushButton )
      {
         Ok();
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class PSFTreeNode : public TreeBox::Node
{
public:

   PSFTreeNode( TreeBox& parent )
      : TreeBox::Node( parent, -1/*append*/ )
   {
   }

   PSFTreeNode( TreeBox::Node& parent )
      : TreeBox::Node( parent, -1/*append*/ )
   {
   }

   virtual ~PSFTreeNode()
   {
   }

   virtual void Update() = 0;
};

// ----------------------------------------------------------------------------

class PSFCollectionNode : public PSFTreeNode
{
public:

   DynamicPSFInterface::PSFCollection* collection;

   PSFCollectionNode( TreeBox& parent, DynamicPSFInterface::PSFCollection* c )
      : PSFTreeNode( parent )
      , collection( c )
   {
      collection->node = this;
      Update();
      Expand();
   }

   virtual ~PSFCollectionNode()
   {
      if ( collection != nullptr )
         collection->node = nullptr;
   }

   void Update() override
   {
      SetIcon( 0, ParentTree().ScaledResource( collection->view.IsNull() ? ":/icons/error.png" :
                        (collection->view.IsPreview() ? ":/browser/preview.png" : ":/browser/main-view.png") ) );
      SetText( 0, collection->ViewId() );
      SetAlignment( 0, TextAlign::Left|TextAlign::VertCenter );
      if ( collection->view.IsNull() )
         SetToolTip( 0, "No such view: " + collection->ViewId() );
   }
};

// ----------------------------------------------------------------------------

class StarNode : public PSFTreeNode
{
public:

   DynamicPSFInterface::Star* star;

   StarNode( TreeBox::Node& parent, DynamicPSFInterface::Star* s )
      : PSFTreeNode( parent )
      , star( s )
   {
      star->node = this;
      Update();
      Expand();
   }

   virtual ~StarNode()
   {
      if ( star != nullptr )
         star->node = nullptr;
   }

   void Update() override
   {
      int sz = ParentTree().Font().Height();
      SetIcon( 0, *star ? Bitmap::FromSVGFile( ":/shapes/star7.svg", sz, sz )
                        : Bitmap( ParentTree().ScaledResource( ":/icons/error.png" ) ) );
      if ( !*star )
         SetToolTip( 0, star->StatusText() );
      SetText( 0, String().Format( "%3d", star->uniqueId ) );
      SetText( 1, String().Format( "%d", star->channel ) );
      SetAlignment( 0, TextAlign::Left|TextAlign::VertCenter );
      SetAlignment( 1, TextAlign::Right|TextAlign::VertCenter );
      // TODO: show also barycenter and selection coordinates?
   }
};

// ----------------------------------------------------------------------------

class PSFNode : public PSFTreeNode
{
public:

   DynamicPSFInterface::PSF* psf;

   PSFNode( TreeBox::Node& parent, DynamicPSFInterface::PSF* p )
      : PSFTreeNode( parent )
      , psf( p )
   {
      psf->node = this;
      Update();
   }

   virtual ~PSFNode()
   {
      if ( psf != nullptr )
         psf->node = nullptr;
   }

   void Update() override
   {
      int sz = RoundInt( 0.8 * ParentTree().Font().Ascent() );
      SetIcon( 0, *psf ? Bitmap::FromSVGFile( ":/shapes/circle.svg", sz, sz )
                       : Bitmap( ParentTree().ScaledResource( ":/icons/error.png" ) ) );
      if ( !*psf )
         SetToolTip( 0, psf->StatusText() );
      SetText( 0, psf->FunctionName() );
      SetAlignment( 0, TextAlign::Left|TextAlign::VertCenter );

      if ( *psf )
      {
         SetText( 2, String().Format( "%8.6f", psf->B ) );
         SetText( 3, String().Format( "%8.6f", psf->A ) );
         SetText( 4, String().Format( "%9.2f", psf->c0.x ) );
         SetText( 5, String().Format( "%9.2f", psf->c0.y ) );

         if ( psf->HasCelestialCoordinates() )
         {
            SetText( 6, String().ToSexagesimal( psf->q0.x/15, RAConversionOptions() ) );
            SetText( 7, String().ToSexagesimal( psf->q0.y, DecConversionOptions() ) );
         }

         SetText( 8, String().Format( "%6.2f", psf->sx ) );

         if ( psf->star->collection->xScale > 0 )
            SetText( 10, String().Format( "%6.2f\"", psf->FWHMx() * psf->star->collection->xScale ) );
         else
            SetText( 10, String().Format( "%6.2fpx", psf->FWHMx() ) );

         if ( !psf->circular )
         {
            SetText( 9, String().Format( "%6.2f", psf->sy ) );

            if ( psf->star->collection->yScale > 0 )
               SetText( 11, String().Format( "%6.2f\"", psf->FWHMy() * psf->star->collection->yScale ) );
            else
               SetText( 11, String().Format( "%6.2fpx", psf->FWHMy() ) );

            SetText( 12, String().Format( "%5.3f", psf->sy/psf->sx ) );
            SetText( 13, String().Format( "%6.2f", (*signedAngles && psf->theta > 90) ? psf->theta - 180 : psf->theta ) );
         }

         SetText( 14, String().Format( "%5.2f", psf->beta ) );
         SetText( 15, String().Format( "%.3e", psf->flux ) );
         SetText( 16, String().Format( "%.3e", psf->meanSignal ) );
         SetText( 17, String().Format( "%.3e", psf->mad ) );

         for ( int i = 2; i <= 17; ++i )
            SetAlignment( i, TextAlign::Right|TextAlign::VertCenter );
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DynamicPSFInterface::PSFCollection::~PSFCollection()
{
   stars.Destroy();
   if ( node != nullptr )
      node->collection = nullptr;
   if ( !view.IsNull() )
      view.RemoveFromDynamicTargets();
}

DynamicPSFInterface::Star::~Star()
{
   psfs.Destroy();
   if ( node != nullptr )
      node->star = nullptr;
}

DynamicPSFInterface::PSF::~PSF()
{
   if ( node != nullptr )
      node->psf = nullptr;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DynamicPSFInterface::DynamicPSFInterface()
   : instance( TheDynamicPSFProcess )
{
   TheDynamicPSFInterface = this;
   signedAngles = &instance.p_signedAngles;
}

// ----------------------------------------------------------------------------

DynamicPSFInterface::~DynamicPSFInterface()
{
   m_selectedStars.Clear();
   m_collections.Destroy();
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString DynamicPSFInterface::Id() const
{
   return "DynamicPSF";
}

// ----------------------------------------------------------------------------

MetaProcess* DynamicPSFInterface::Process() const
{
   return TheDynamicPSFProcess;
}

// ----------------------------------------------------------------------------

String DynamicPSFInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/DynamicPSF.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures DynamicPSFInterface::Features() const
{
   return InterfaceFeature::DragObject |
          InterfaceFeature::CancelButton |
          InterfaceFeature::BrowseDocumentationButton |
          InterfaceFeature::ResetButton;
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::ResetInstance()
{
   DynamicPSFInstance defaultInstance( TheDynamicPSFProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool DynamicPSFInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& flags )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      UpdateStyleSheets();
      SetWindowTitle( "DynamicPSF" );
      UpdateControls();
      RegenerateDataTree();
   }

   dynamic = true;
   return &P == TheDynamicPSFProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* DynamicPSFInterface::NewProcess() const
{
   DynamicPSFInstance* exportInstance = new DynamicPSFInstance( TheDynamicPSFProcess );
   exportInstance->AssignOptions( instance );
   for ( const PSFCollection& collection : m_collections )
   {
      exportInstance->p_views << collection.ViewId();
      for ( const Star& star : collection.stars )
      {
         exportInstance->p_stars << DynamicPSFInstance::Star( star, exportInstance->p_views.Length()-1 );
         for ( const PSF& psf : star.psfs )
            exportInstance->p_psfs << DynamicPSFInstance::PSF( psf, exportInstance->p_stars.Length()-1 );
      }
   }

   return exportInstance;
}

// ----------------------------------------------------------------------------

bool DynamicPSFInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const DynamicPSFInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a DynamicPSF instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool DynamicPSFInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool DynamicPSFInterface::ImportProcess( const ProcessImplementation& P )
{
   const DynamicPSFInstance* importInstance = dynamic_cast<const DynamicPSFInstance*>( &P );
   if ( importInstance == nullptr )
      throw Error( "Not a DynamicPSF instance." );

   Update();

   m_selectedStars.Clear();
   m_collections.Destroy();

   GUI->Data_TreeBox.Clear();

   instance.AssignOptions( *importInstance );

   for ( size_type i = 0; i < importInstance->p_views.Length(); ++i )
   {
      PSFCollection* collection = new PSFCollection( importInstance->p_views[i], importInstance->p_astrometry );
      m_collections << collection;
      collection->UpdateImageScale( instance.p_scaleMode, instance.p_scaleValue, instance.p_scaleKeyword );

      for ( size_type j = 0; j < importInstance->p_stars.Length(); ++j )
      {
         const DynamicPSFInstance::Star& star = importInstance->p_stars[j];
         if ( star.view == i )
         {
            Star* newStar = collection->AddStar( star );
            for ( size_type k = 0; k < importInstance->p_psfs.Length(); ++k )
            {
               const DynamicPSFInstance::PSF& psf = importInstance->p_psfs[k];
               if ( psf.star == j )
                  newStar->AddPSF( psf );
            }
         }
      }
   }

   for ( PSFCollection& collection : m_collections )
      collection.Recalculate( instance.p_threshold, instance.p_autoAperture, instance.p_psfOptions );

   RegenerateDataTree();
   Update();
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

bool DynamicPSFInterface::IsDynamicInterface() const
{
   return true;
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::ExitDynamicMode()
{
   Update();

   m_selectedStars.Clear();
   m_collections.Destroy();

   GUI->Data_TreeBox.Clear();
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::DynamicMouseEnter( View& view )
{
   // Placeholder
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::DynamicMouseLeave( View& view )
{
   // Placeholder
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::DynamicMousePress( View& view, const DPoint& pos, int button, unsigned buttons, unsigned modifiers )
{
   AcquirePSFCollection( view );
   if ( !(modifiers & KeyModifier::Shift) )
      UnselectStars();
   ImageWindow window = view.Window();
   window.BeginSelection( pos.RoundedToInt() );
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::DynamicMouseMove( View& view, const DPoint& pos, unsigned buttons, unsigned modifiers )
{
   ImageWindow window = view.Window();
   if ( window.IsSelection() )
   {
      window.ModifySelection( pos.RoundedToInt() );
      star_list stars = FindStars( view, window.SelectionRect(), window.CurrentChannel() );
      SelectStars( stars, modifiers & KeyModifier::Shift/*addToSelection*/ );
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::DynamicMouseRelease( View& view, const DPoint& pos, int button, unsigned buttons, unsigned modifiers )
{
   ImageWindow window = view.Window();

   if ( !window.IsSelection() )
      return;

   window.ModifySelection( pos.RoundedToInt() );
   DRect rect = window.SelectionRect();
   window.EndSelection();

   ImageWindow::display_channel channel = window.CurrentChannel();

   PSFCollection* collection = AcquirePSFCollection( view );

   star_list stars = FindStars( view, rect, channel );

   String message;

   if ( stars.IsEmpty() )
      if ( !rect.IsRect() )
      {
         for ( int ch = 0; ch < collection->image.NumberOfNominalChannels(); ++ch )
         {
            switch ( channel )
            {
            case DisplayChannel::Red:
               if ( ch != 0 )
                  continue;
               break;
            case DisplayChannel::Green:
               if ( ch != 1 )
                  continue;
               break;
            case DisplayChannel::Blue:
               if ( ch != 2 )
                  continue;
               break;
            default:
               break;
            }

            StarDetector D( collection->image, ch, pos, instance.p_searchRadius, instance.p_threshold, instance.p_autoAperture );
            if ( D )
            {
               Star* star = collection->AddStar( D.star );
               star->Regenerate( instance.p_psfOptions );
               new StarNode( *collection->node, star );
               star->CreatePSFNodes();
               stars << star;
               if ( star->psfs.IsEmpty() )
                  message = "NO CONVERGENCE";
            }
            else
            {
               message = D.star.StatusText().Uppercase();
            }
         }

         AdjustDataTreeColumns();
      }

   if ( !stars.IsEmpty() )
   {
      for ( Star& star : stars )
      {
         star.node->Expand();
         for ( const PSF& psf : star.psfs )
            GUI->Data_TreeBox.SetNodeIntoView( psf.node );
      }
      GUI->Data_TreeBox.SetCurrentNode( stars.Begin()->node );
   }

   SelectStars( stars, modifiers & KeyModifier::Shift/*addToSelection*/ );

   UpdateControls();

   if ( !message.IsEmpty() )
      GUI->StarInfo_Label.SetText( "<* " + message + " *>" );
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::DynamicMouseDoubleClick( View& view, const DPoint& pos, unsigned buttons, unsigned modifiers )
{
   // Placeholder
}

// ----------------------------------------------------------------------------

bool DynamicPSFInterface::DynamicKeyPress( View& view, int key, unsigned modifiers )
{
   if ( !view.IsDynamicTarget() )
      return false;

   switch ( key )
   {
   case KeyCode::Delete:
   case KeyCode::Backspace:
      // Simulate a button click
      __Click( GUI->DeleteStar_ToolButton, false );
      break;

   case KeyCode::Escape:
      //ImageWindow::TerminateDynamicSession();
      break;

   default:
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool DynamicPSFInterface::RequiresDynamicUpdate( const View& view, const DRect& rect ) const
{
   const PSFCollection* collection = FindPSFCollection( view );
   if ( collection != nullptr )
   {
      ImageWindow window = view.Window();
      ImageWindow::display_channel channel = window.CurrentChannel();

      Rect vrect = window.ImageToViewport( rect ).TruncatedToInt().InflatedBy( 1 );
      for ( const Star& star : collection->stars )
      {
         switch ( channel )
         {
         case DisplayChannel::Red:
            if ( star.channel != 0 )
               continue;
            break;
         case DisplayChannel::Green:
            if ( star.channel != 1 )
               continue;
            break;
         case DisplayChannel::Blue:
            if ( star.channel != 2 )
               continue;
            break;
         default:
            break;
         }

         Rect srect = window.ImageToViewport( star.rect ).TruncatedToInt().InflatedBy( 1 );
         if ( vrect.Intersects( srect ) )
            return true;
      }
   }
   return false;
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::DynamicPaint( const View& view, VectorGraphics& g, const DRect& rect ) const
{
   const PSFCollection* collection = FindPSFCollection( view );
   if ( collection == nullptr ) // should not happen!
      return;

   ImageWindow window = view.Window();
   ImageWindow::display_channel channel = window.CurrentChannel();

   // Update rectangle, integer viewport coordinates.
   // Inflated rectangle coordinates compensate for rounding errors.
   Rect r0 = window.ImageToViewport( rect ).TruncatedToInt().InflatedBy( 1 );

   g.SetCompositionOperator( CompositionOp::Difference );

   // Draw stars and their PSFs
   double penWidth = DisplayPixelRatio();
   for ( const Star& star : collection->stars )
   {
      switch ( channel )
      {
      case DisplayChannel::Red:
         if ( star.channel != 0 )
            continue;
         break;
      case DisplayChannel::Green:
         if ( star.channel != 1 )
            continue;
         break;
      case DisplayChannel::Blue:
         if ( star.channel != 2 )
            continue;
         break;
      default:
         break;
      }
      DrawStar( g, penWidth, star, window, r0 );
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::SaveSettings() const
{
   // Placeholder
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::LoadSettings()
{
   // Placeholder
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::ExecuteInstance( DynamicPSFInstance& instance )
{
   psf_collection_list collections;

   for ( size_type i = 0; i < instance.p_views.Length(); ++i )
   {
      PSFCollection* collection = new PSFCollection( instance.p_views[i], instance.p_astrometry );
      collections << collection;

      for ( size_type j = 0; j < instance.p_stars.Length(); ++j )
      {
         const DynamicPSFInstance::Star& star = instance.p_stars[j];
         if ( star.view == i )
         {
            Star* newStar = collection->AddStar( star );

            for ( size_type k = 0; k < instance.p_psfs.Length(); ++k )
            {
               const DynamicPSFInstance::PSF& psf = instance.p_psfs[k];
               if ( psf.star == j )
                  newStar->AddPSF( psf );
            }
         }
      }
   }

   instance.p_views.Clear();
   instance.p_stars.Clear();
   instance.p_psfs.Clear();

   for ( PSFCollection& collection : collections )
      if ( instance.p_regenerate )
         collection.Regenerate( instance.p_threshold, instance.p_autoAperture, instance.p_psfOptions );
      else
         collection.Recalculate( instance.p_threshold, instance.p_autoAperture, instance.p_psfOptions );

   for ( const PSFCollection& collection : collections )
   {
      instance.p_views << collection.ViewId();

      for ( const Star& star : collection.stars )
      {
         instance.p_stars << DynamicPSFInstance::Star( star, instance.p_views.Length()-1 );

         for ( const PSF& psf : star.psfs )
            instance.p_psfs << DynamicPSFInstance::PSF( psf, instance.p_stars.Length()-1 );
      }
   }

   collections.Destroy();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DynamicPSFInterface::UpdateControls()
{
   bool haveStars = !m_collections.IsEmpty();
   GUI->ExpandAll_ToolButton.Enable( haveStars );
   GUI->CollapseAll_ToolButton.Enable( haveStars );
   GUI->DeleteStar_ToolButton.Enable( haveStars );
   GUI->TrackStars_ToolButton.Enable( haveStars );
   GUI->Regenerate_ToolButton.Enable( haveStars );
   GUI->RegenerateAll_ToolButton.Enable( haveStars );
   GUI->Sort_ToolButton.Enable( haveStars );
   GUI->ExportPSF_ToolButton.Enable( haveStars );
   GUI->AverageStars_ToolButton.Enable( haveStars );
   GUI->ExportCSV_ToolButton.Enable( haveStars );

   bool fixedPSF = !instance.p_psfOptions.autoPSF;
   GUI->Gaussian_CheckBox.Enable( fixedPSF );
   GUI->VariableShape_CheckBox.Enable( fixedPSF );
   GUI->Moffat_CheckBox.Enable( fixedPSF );
   GUI->MoffatA_CheckBox.Enable( fixedPSF );
   GUI->Moffat8_CheckBox.Enable( fixedPSF );
   GUI->Moffat6_CheckBox.Enable( fixedPSF );
   GUI->Moffat4_CheckBox.Enable( fixedPSF );
   GUI->Moffat25_CheckBox.Enable( fixedPSF );
   GUI->Moffat15_CheckBox.Enable( fixedPSF );
   GUI->Lorentzian_CheckBox.Enable( fixedPSF );

   GUI->AutoPSF_CheckBox.SetChecked( !fixedPSF );
   GUI->Gaussian_CheckBox.SetChecked( instance.p_psfOptions.gaussian );
   GUI->VariableShape_CheckBox.SetChecked( instance.p_psfOptions.variableShape );
   GUI->Moffat_CheckBox.SetChecked( instance.p_psfOptions.moffat );
   GUI->MoffatA_CheckBox.SetChecked( instance.p_psfOptions.moffatA );
   GUI->Moffat8_CheckBox.SetChecked( instance.p_psfOptions.moffat8 );
   GUI->Moffat6_CheckBox.SetChecked( instance.p_psfOptions.moffat6 );
   GUI->Moffat4_CheckBox.SetChecked( instance.p_psfOptions.moffat4 );
   GUI->Moffat25_CheckBox.SetChecked( instance.p_psfOptions.moffat25 );
   GUI->Moffat15_CheckBox.SetChecked( instance.p_psfOptions.moffat15 );
   GUI->Lorentzian_CheckBox.SetChecked( instance.p_psfOptions.lorentzian );
   GUI->CircularPSF_CheckBox.SetChecked( instance.p_psfOptions.circular );
   GUI->SignedAngles_CheckBox.SetChecked( instance.p_signedAngles );
   GUI->AutoVariableShapePSF_CheckBox.SetChecked( instance.p_psfOptions.autoVariableShapePSF );

   GUI->SearchRadius_SpinBox.SetValue( instance.p_searchRadius );
   GUI->Threshold_NumericEdit.SetValue( instance.p_threshold );
   GUI->AutoAperture_CheckBox.SetChecked( instance.p_autoAperture );

   GUI->ScaleMode_ComboBox.SetCurrentItem( instance.p_scaleMode );

   GUI->ScaleValue_NumericEdit.SetValue( instance.p_scaleValue );
   GUI->ScaleValue_NumericEdit.Enable( instance.p_scaleMode == DPScaleMode::LiteralValue );

   GUI->ScaleKeyword_Edit.SetText( instance.p_scaleKeyword );
   GUI->ScaleKeyword_Label.Enable( instance.p_scaleMode == DPScaleMode::CustomKeyword );
   GUI->ScaleKeyword_Edit.Enable( instance.p_scaleMode == DPScaleMode::CustomKeyword );

   UpdateStarInfo();
}

// ----------------------------------------------------------------------------

static double KeywordValue( const FITSKeywordArray& keywords, const IsoString& keyName )
{
   for ( const FITSHeaderKeyword& keyword : keywords )
      if ( !keyword.name.CompareIC( keyName ) )
      {
         if ( keyword.IsNumeric() )
         {
            double v;
            if ( keyword.GetNumericValue( v ) )
               return v;
         }

         break;
      }

   return 0;
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::UpdateScaleItems()
{
   for ( PSFCollection& collection : m_collections )
      collection.UpdateImageScale( instance.p_scaleMode, instance.p_scaleValue, instance.p_scaleKeyword );
   AdjustDataTreeColumns();
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::UpdateStarInfo()
{
   TreeBox::Node* node = GUI->Data_TreeBox.CurrentNode();
   if ( node == nullptr )
      GUI->StarInfo_Label.Clear();
   else
   {
      StarNode* s = dynamic_cast<StarNode*>( node );
      if ( s == nullptr )
      {
         PSFNode* psfNode = dynamic_cast<PSFNode*>( node );
         if ( psfNode != nullptr )
            s = dynamic_cast<StarNode*>( node->Parent() );
      }

      if ( s != nullptr )
      {
         String info;
         info.Format( "Star %5d of %5d",
                      GUI->Data_TreeBox.ChildIndex( s ) + 1,
                      s->Parent()->NumberOfChildren() );
         if ( !m_selectedStars.IsEmpty() )
            info.AppendFormat( " / %u selected", m_selectedStars.Length() );
         GUI->StarInfo_Label.SetText( info );
      }
      else
      {
         PSFCollectionNode* c = dynamic_cast<PSFCollectionNode*>( node );
         if ( c != nullptr )
         {
            String info = String().Format( "%5u stars", c->collection->stars.Length() );
            if ( c->collection->xScale > 0 && c->collection->yScale > 0 )
               info += String().Format( " | x-scale=%.2f \"/px | y-scale=%.2f \"/px",
                                        c->collection->xScale, c->collection->yScale );
            GUI->StarInfo_Label.SetText( info );
         }
         else
            GUI->StarInfo_Label.Clear();
      }
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::UpdateStyleSheets()
{
   GUI->Data_TreeBox.SetStyleSheet( ScaledStyleSheet( String().Format(
         "QTreeView {"
            "font-family: Hack, DejaVu Sans Mono, Monospace;"
            "font-size: %.2fpt;"
         "}"
         "QTreeView::item {"
            "height: %.2fpt;"
         "}"
         , m_fontSize, 1.2*m_fontSize ) ) );

   int iconSize = Max( 16, RoundInt( m_fontSize*16/9 ) );
   GUI->Data_TreeBox.SetScaledIconSize( iconSize, iconSize );
   GUI->Data_TreeBox.Restyle();
   AdjustDataTreeColumns();

   String fontSizeTip = String().Format( "<br/>(now %.2fpt)", m_fontSize );
   GUI->EnlargeFont_ToolButton.SetToolTip( "<p>Enlarge font " + fontSizeTip + "</p>" );
   GUI->ShrinkFont_ToolButton.SetToolTip( "<p>Shrink font " + fontSizeTip + "</p>" );

   GUI->EnlargeFont_ToolButton.Enable( m_fontSize < MAX_FONT_SIZE );
   GUI->ShrinkFont_ToolButton.Enable( m_fontSize > MIN_FONT_SIZE );
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::RegenerateDataTree()
{
   GUI->Data_TreeBox.Clear();
   GUI->Data_TreeBox.DisableUpdates();

   for ( PSFCollection& collection : m_collections )
   {
      new PSFCollectionNode( GUI->Data_TreeBox, &collection );
      collection.CreateStarNodes();
   }

   GUI->Data_TreeBox.EnableUpdates();

   AdjustDataTreeColumns();
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::AdjustDataTreeColumns()
{
   for ( int i = 0, n = GUI->Data_TreeBox.NumberOfColumns(); i < n; ++i )
      GUI->Data_TreeBox.AdjustColumnWidthToContents( i );
}


// ----------------------------------------------------------------------------

void DynamicPSFInterface::SetPSFunction( PSFData::psf_function f, bool onOff )
{
   switch ( f )
   {
   default:
   case PSFunction::Gaussian:
      instance.p_psfOptions.gaussian = onOff;
      break;
   case PSFunction::Moffat:
      instance.p_psfOptions.moffat = onOff;
      break;
   case PSFunction::MoffatA:
      instance.p_psfOptions.moffatA = onOff;
      break;
   case PSFunction::Moffat8:
      instance.p_psfOptions.moffat8 = onOff;
      break;
   case PSFunction::Moffat6:
      instance.p_psfOptions.moffat6 = onOff;
      break;
   case PSFunction::Moffat4:
      instance.p_psfOptions.moffat4 = onOff;
      break;
   case PSFunction::Moffat25:
      instance.p_psfOptions.moffat25 = onOff;
      break;
   case PSFunction::Moffat15:
      instance.p_psfOptions.moffat15 = onOff;
      break;
   case PSFunction::Lorentzian:
      instance.p_psfOptions.lorentzian = onOff;
      break;
   case PSFunction::VariableShape:
      instance.p_psfOptions.variableShape = onOff;
      break;
   }
   instance.p_psfOptions.Validate();
   UpdateControls();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DynamicPSFInterface::__CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   UpdateStarInfo();

   if ( GUI->TrackStars_ToolButton.IsChecked() )
      TrackStar( StarFromTreeBoxNode( current ) );
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::__NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   Star* star = StarFromTreeBoxNode( node );
   if ( star != nullptr )
   {
      TrackStar( star );
      SelectStar( star );
   }
   else
   {
      PSFCollectionNode* collectionNode = dynamic_cast<PSFCollectionNode*>( &node );
      if ( collectionNode != nullptr )
      {
         collectionNode->collection->view.Window().BringToFront();
         collectionNode->collection->view.Window().SelectView( collectionNode->collection->view );
      }
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::__NodeDoubleClicked( TreeBox& sender, TreeBox::Node& node, int col )
{
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::__NodeExpanded( TreeBox& sender, TreeBox::Node& node )
{
   AdjustDataTreeColumns();
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::__NodeCollapsed( TreeBox& sender, TreeBox::Node& node )
{
   // Placeholder
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::__NodeSelectionUpdated( TreeBox& sender )
{
   star_list stars;
   IndirectArray<TreeBox::Node> selectedNodes = sender.SelectedNodes();
   for ( TreeBox::Node* node : selectedNodes )
   {
      Star* star = StarFromTreeBoxNode( *node );
      if ( star != nullptr )
         if ( !stars.Contains( star ) )
            stars.Add( star );
   }
   SelectStars( stars );
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->EnlargeFont_ToolButton )
   {
      if ( m_fontSize < MAX_FONT_SIZE )
      {
         m_fontSize += 0.5;
         UpdateStyleSheets();
      }
   }
   else if ( sender == GUI->ShrinkFont_ToolButton )
   {
      if ( m_fontSize > MIN_FONT_SIZE )
      {
         m_fontSize -= 0.5;
         UpdateStyleSheets();
      }
   }
   else if ( sender == GUI->ExpandAll_ToolButton )
   {
      for ( int n = GUI->Data_TreeBox.NumberOfChildren(), i = n; --i >= 0; )
         ExpandTreeBoxNodes( GUI->Data_TreeBox[i] );
      AdjustDataTreeColumns();
   }
   else if ( sender == GUI->CollapseAll_ToolButton )
   {
      IndirectArray<TreeBox::Node> nodes = GUI->Data_TreeBox.SelectedNodes();
      if ( !nodes.IsEmpty() )
      {
         // Collapse selected view and star nodes.
         for ( TreeBox::Node* node : nodes )
            if ( node->Parent() == nullptr || node->Parent()->Parent() == nullptr )
               node->Collapse();
      }
      else
      {
         // Collapse all top-level nodes.
         for ( int n = GUI->Data_TreeBox.NumberOfChildren(), i = n; --i >= 0; )
            CollapseTreeBoxNodes( GUI->Data_TreeBox[i] );
      }
      AdjustDataTreeColumns();
   }
   else if ( sender == GUI->DeleteStar_ToolButton )
   {
      IndirectArray<TreeBox::Node> selectedNodes = GUI->Data_TreeBox.SelectedNodes();
      if ( selectedNodes.IsEmpty() )
      {
         pcl::MessageBox( "<p>There are no selected items to delete.</p>",
                           "DynamicPSF",
                           StdIcon::Error ).Execute();
      }
      else
      {
         ReferenceArray<PSF>           psfs;
         ReferenceArray<Star>          stars;
         ReferenceArray<PSFCollection> collections;

         {
            IndirectArray<TreeBox::Node> psfNodes;
            IndirectArray<TreeBox::Node> starNodes;
            IndirectArray<TreeBox::Node> collectionNodes;

            for ( TreeBox::Node* node : selectedNodes )
            {
               PSFNode* psfNode = dynamic_cast<PSFNode*>( node );
               if ( psfNode != nullptr )
               {
                  psfs << psfNode->psf;
                  psfNodes << node;
               }
               else
               {
                  StarNode* starNode = dynamic_cast<StarNode*>( node );
                  if ( starNode != nullptr )
                  {
                     stars << starNode->star;
                     starNodes << node;
                  }
                  else
                  {
                     PSFCollectionNode* psfCollectionNode = dynamic_cast<PSFCollectionNode*>( node );
                     if ( psfCollectionNode != nullptr )
                     {
                        collections << psfCollectionNode->collection;
                        collectionNodes << node;
                     }
                  }
               }
            }

            GUI->Data_TreeBox.DisableUpdates();

            psfNodes.Destroy();
            starNodes.Destroy();
            collectionNodes.Destroy();

            GUI->Data_TreeBox.EnableUpdates();
         }

         for ( PSFCollection& collection : collections )
         {
            for ( Star& star : collection.stars )
            {
               if ( star.selected )
                  m_selectedStars.Remove( m_selectedStars.Search( &star ) );

               for ( PSF& psf : star.psfs )
                  psfs.Remove( psfs.Search( &psf ) );

               stars.Remove( stars.Search( &star ) );
            }

            collection.Update();
            m_collections.Destroy( m_collections.Search( &collection ) );
         }

         for ( Star& star : stars )
         {
            if ( star.selected )
               m_selectedStars.Remove( m_selectedStars.Search( &star ) );

            for ( PSF& psf : star.psfs )
               psfs.Remove( psfs.Search( &psf ) );

            star.Update();
            star.collection->stars.Destroy( star.collection->stars.Search( &star ) );
         }

         for ( PSF& psf : psfs )
         {
            psf.star->Update();
            psf.star->psfs.Destroy( psf.star->psfs.Search( &psf ) );
         }

         TreeBox::Node* current = GUI->Data_TreeBox.CurrentNode();
         if ( current != nullptr )
         {
            current->Select();
            SelectStar( StarFromTreeBoxNode( *current ) );
         }

         UpdateControls();
      }
   }
   else if ( sender == GUI->TrackStars_ToolButton )
   {
      if ( checked )
      {
         TreeBox::Node* current = GUI->Data_TreeBox.CurrentNode();
         if ( current != nullptr )
            TrackStar( StarFromTreeBoxNode( *current ) );
      }
   }
   else if ( sender == GUI->Regenerate_ToolButton )
   {
      if ( m_selectedStars.IsEmpty() )
      {
         pcl::MessageBox( "<p>There are no selected stars to regenerate.</p>",
                           "DynamicPSF",
                           StdIcon::Error ).Execute();
      }
      else
      {
         GUI->StarInfo_Label.SetText( "Regenerating selection..." );
         ProcessEvents();

         GUI->Data_TreeBox.DisableUpdates();

         for ( Star& star : m_selectedStars )
         {
            star.DestroyPSFNodes();
            star.Regenerate( instance.p_threshold, instance.p_autoAperture, instance.p_psfOptions );
            star.CreatePSFNodes();
            star.Update();
         }

         GUI->Data_TreeBox.EnableUpdates();

         GUI->StarInfo_Label.Clear();
      }
   }
   else if ( sender == GUI->RegenerateAll_ToolButton )
   {
      star_list wereSelected = m_selectedStars;
      UnselectStars();

      GUI->StarInfo_Label.SetText( "Regenerating..." );
      ProcessEvents();

      for ( PSFCollection& collection : m_collections )
         collection.Regenerate( instance.p_threshold, instance.p_autoAperture, instance.p_psfOptions );

      RegenerateDataTree();

      GUI->StarInfo_Label.Clear();

      Update();
      UpdateControls();
      SelectStars( wereSelected );
   }
   else if ( sender == GUI->Sort_ToolButton )
   {
      if ( s_sortStarsDialog == nullptr )
         s_sortStarsDialog = new SortStarsDialog;
      if ( s_sortStarsDialog->Execute() )
      {
         GUI->StarInfo_Label.SetText( "Sorting..." );
         ProcessEvents();

         for ( PSFCollection& collection : m_collections )
            collection.Sort( s_sortStarsDialog->sortBy );

         RegenerateDataTree();

         GUI->StarInfo_Label.Clear();

         for ( Star& star : m_selectedStars )
            if ( star.node != nullptr )
               star.node->Select();
      }
   }
   else if ( sender == GUI->ExportPSF_ToolButton )
   {
      if ( m_selectedStars.IsEmpty() )
      {
         pcl::MessageBox( "<p>There are no selected stars to generate a synthetic PSF.</p>",
                           "DynamicPSF",
                           StdIcon::Error ).Execute();
         return;
      }

      FMatrix R;
      for ( Star& star : m_selectedStars )
      {
         for ( PSF& psf : star.psfs )
         {
            FMatrix r;
            if ( psf.function == PSFunction::Gaussian || psf.function == PSFunction::VariableShape )
               r = VariableShapeFilter( float( psf.sx ), psf.beta, 0.01F/*eps*/, psf.sy/psf.sx/*rho*/, Rad( psf.theta ) ).Coefficients();
            else // Moffat
               r = MoffatFilter( float( psf.sx ), psf.beta, 0.01F/*eps*/, psf.sy/psf.sx/*rho*/, Rad( psf.theta ) ).Coefficients();

            if ( R.Rows() < r.Rows() )
               Swap( R, r );

            for ( int i0 = 0, k = (R.Rows()-r.Rows())>>1, i = k; i0 < r.Rows(); ++i0, ++i )
               for ( int j0 = 0, j = k; j0 < r.Cols(); ++j0, ++j )
                  R[i][j] += r[i0][j0];
         }
      }

      ImageWindow window( 1, 1, 1, 32, true/*floatSample*/, false/*color*/, true/*initialProcessing*/, "PSF" );
      ImageVariant image = window.MainView().Image();
      R.ToImage( image );
      image.Rescale();
      window.Show();
   }
   else if ( sender == GUI->AverageStars_ToolButton )
   {
      if ( m_selectedStars.IsEmpty() )
      {
         pcl::MessageBox( "<p>There are no selected stars to average.</p>",
                           "DynamicPSF",
                           StdIcon::Error ).Execute();
         return;
      }

      PSFCollection* collection = nullptr;
      int variableShape = -1;
      int count = 0;
      for ( Star& star : m_selectedStars )
      {
         if ( collection == nullptr )
            collection = star.collection;
         else if ( star.collection != collection )
         {
            pcl::MessageBox( "<p>Cannot average stars from different images.</p>",
                              "DynamicPSF",
                              StdIcon::Error ).Execute();
            return;
         }

         for ( PSF& psf : star.psfs )
            if ( psf )
            {
               bool thisVariableShape = psf.function == PSFunction::Gaussian || psf.function == PSFunction::VariableShape;
               if ( variableShape < 0 )
                  variableShape = thisVariableShape;
               else if ( variableShape != thisVariableShape )
               {
                  pcl::MessageBox( "<p>Incongruent PSF functions. Cannot average Gaussian/VariableShape and Moffat functions.</p>",
                                   "DynamicPSF",
                                   StdIcon::Error ).Execute();
                  return;
               }

               ++count;
            }
      }

      if ( count == 0 )
      {
         pcl::MessageBox( "<p>There are no valid PSF functions among the selected stars.</p>",
                          "DynamicPSF",
                          StdIcon::Error ).Execute();
         return;
      }

      AverageStarDialog( m_selectedStars, collection->xScale, collection->yScale ).Execute();
   }
   else if ( sender == GUI->ExportCSV_ToolButton )
   {
      SaveFileDialog d;
      d.SetCaption( "DynamicPSF: Export As CSV File" );
      d.SetFilters( FileDialog::filter_list() << FileFilter( "CSV Files", ".csv" )
                                              << FileFilter( "Plain Text Files", ".txt" )
                                              << FileFilter( "Any Files", "*" ) );
      d.EnableOverwritePrompt();
      if ( d.Execute() )
         ExportCSV( d.FileName() );
   }
   else if ( sender == GUI->AutoPSF_CheckBox )
   {
      instance.p_psfOptions.autoPSF = checked;
      if ( !instance.p_psfOptions.autoPSF )
         if ( !instance.p_psfOptions.Validate() )
         {
            instance.p_psfOptions.autoPSF = false;
            instance.p_psfOptions.gaussian = true;
         }
      UpdateControls();
   }
   else if ( sender == GUI->Gaussian_CheckBox )
      SetPSFunction( PSFunction::Gaussian, checked );
   else if ( sender == GUI->VariableShape_CheckBox )
      SetPSFunction( PSFunction::VariableShape, checked );
   else if ( sender == GUI->Moffat_CheckBox )
      SetPSFunction( PSFunction::Moffat, checked );
   else if ( sender == GUI->MoffatA_CheckBox )
      SetPSFunction( PSFunction::MoffatA, checked );
   else if ( sender == GUI->Moffat8_CheckBox )
      SetPSFunction( PSFunction::Moffat8, checked );
   else if ( sender == GUI->Moffat6_CheckBox )
      SetPSFunction( PSFunction::Moffat6, checked );
   else if ( sender == GUI->Moffat4_CheckBox )
      SetPSFunction( PSFunction::Moffat4, checked );
   else if ( sender == GUI->Moffat25_CheckBox )
      SetPSFunction( PSFunction::Moffat25, checked );
   else if ( sender == GUI->Moffat15_CheckBox )
      SetPSFunction( PSFunction::Moffat15, checked );
   else if ( sender == GUI->Lorentzian_CheckBox )
      SetPSFunction( PSFunction::Lorentzian, checked );
   else if ( sender == GUI->CircularPSF_CheckBox )
      instance.p_psfOptions.circular = checked;
   else if ( sender == GUI->SignedAngles_CheckBox )
   {
      instance.p_signedAngles = checked;
      for ( PSFCollection& collection : m_collections )
         collection.UpdateNodes();
      AdjustDataTreeColumns();
   }
   else if ( sender == GUI->AutoVariableShapePSF_CheckBox )
      instance.p_psfOptions.autoVariableShapePSF = checked;
   else if ( sender == GUI->AutoAperture_CheckBox )
      instance.p_autoAperture = checked;
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::__NumericEdit_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Threshold_NumericEdit )
      instance.p_threshold = value;
   else if ( sender == GUI->ScaleValue_NumericEdit )
   {
      instance.p_scaleValue = value;
      UpdateControls();
      UpdateScaleItems();
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::__SpinBox_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->SearchRadius_SpinBox )
      instance.p_searchRadius = value;
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->ScaleMode_ComboBox )
   {
      instance.p_scaleMode = itemIndex;
      UpdateControls();
      UpdateScaleItems();
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::__EditCompleted( Edit& sender )
{
   if ( sender == GUI->ScaleKeyword_Edit )
   {
      instance.p_scaleKeyword = sender.Text().Trimmed();
      UpdateControls();
      UpdateScaleItems();
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
      GUI->Data_TreeBox.SetFixedHeight();
   else
   {
      GUI->Data_TreeBox.SetMinHeight( DATATREE_MINHEIGHT( Font() ) );
      GUI->Data_TreeBox.SetMaxHeight( int_max );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DynamicPSFInterface::PSFCollection* DynamicPSFInterface::AcquirePSFCollection( View& view )
{
   PSFCollection* collection = FindPSFCollection( view );
   if ( collection == nullptr )
   {
      m_collections.Add( collection = new PSFCollection( view, instance.p_astrometry ) );
      collection->UpdateImageScale( instance.p_scaleMode, instance.p_scaleValue, instance.p_scaleKeyword );
      new PSFCollectionNode( GUI->Data_TreeBox, collection );
   }
   return collection;
}

// ----------------------------------------------------------------------------

DynamicPSFInterface::star_list DynamicPSFInterface::FindStars( const View& view,
                                             const DRect& rect, ImageWindow::display_channel channel )
{
   star_list stars;
   PSFCollection* collection = FindPSFCollection( view );
   if ( collection != nullptr )
      for ( Star& star : collection->stars )
      {
         switch ( channel )
         {
         case DisplayChannel::Red:
            if ( star.channel != 0 )
               continue;
            break;
         case DisplayChannel::Green:
            if ( star.channel != 1 )
               continue;
            break;
         case DisplayChannel::Blue:
            if ( star.channel != 2 )
               continue;
            break;
         default:
            break;
         }

         if ( star.rect.Intersects( rect ) )
            stars << &star;
      }
   return stars;
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::SelectStars( const star_list& stars, bool addToSelection )
{
   if ( addToSelection )
   {
      for ( const Star& star : stars )
         if ( !star.selected )
            m_selectedStars << &star;
   }
   else
   {
      for ( Star& star : m_selectedStars )
      {
         star.selected = false;
         star.Update();
         if ( star.node != nullptr )
            star.node->Unselect();
      }

      m_selectedStars = stars;
   }

   for ( Star& star : m_selectedStars )
   {
      star.selected = true;
      star.Update();
      if ( star.node != nullptr )
         star.node->Select();
   }

   UpdateStarInfo();
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::TrackStar( const Star* star )
{
   if ( star != nullptr )
   {
      ImageWindow window = star->collection->view.Window();
      window.SelectView( star->collection->view );
      window.SetViewport( star->pos, window.ZoomFactor() );
      if ( window != ImageWindow::ActiveWindow() )
      {
         window.BringToFront();
         ActivateWindow();
      }
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::ExportCSV( const String& filePath )
{
   File f = File::CreateFileForWriting( filePath );

   f.OutTextLn( "ViewId,StarId,Channel,Function,B,A,cx,cy,alpha,delta,sx,sy,FWHMx,FWHMy,unit,r,theta,beta,flux,meanSignal,MAD" );

   for ( PSFCollection& collection : m_collections )
   {
      IsoString viewId = collection.ViewId();

      const char* scaleUnit = "px";
      double xScale = 1, yScale = 1;
      if ( collection.xScale > 0 )
         if ( collection.yScale > 0 )
         {
            scaleUnit = "as";
            xScale = collection.xScale;
            yScale = collection.yScale;
         }

      for ( Star& star : collection.stars )
      {
         IsoString viewIdAndStarIdAndCh = viewId + ',' + IsoString( star.uniqueId ) + ',' + IsoString( star.channel ) + ',';
         for ( PSF& psf : star.psfs )
         {
            IsoString alpha, delta;
            if ( psf.HasCelestialCoordinates() )
            {
               alpha = IsoString().ToSexagesimal( psf.q0.x/15, RAConversionOptions( 3/*precision*/, 0/*width*/ ) );
               delta = IsoString().ToSexagesimal( psf.q0.y, DecConversionOptions( 2/*precision*/, 0/*width*/ ) );
            }

            f.OutTextLn( viewIdAndStarIdAndCh +
                         IsoString( psf.FunctionName() ) + ',' +
                         IsoString().Format( "%.6f,%.6f,%.2f,%.2f,%s,%s,%.2f,%.2f,%.2f,%.2f,%s,%.3f,%.2f,%.2f,%.3e,%.3e,%.3e",
                                             psf.B,
                                             psf.A,
                                             psf.c0.x,
                                             psf.c0.y,
                                             alpha.c_str(),
                                             delta.c_str(),
                                             psf.sx,
                                             psf.sy,
                                             psf.FWHMx()*xScale,
                                             psf.FWHMy()*yScale,
                                             scaleUnit,
                                             psf.sy/psf.sx,
                                             (*signedAngles && psf.theta > 90) ? psf.theta - 180 : psf.theta,
                                             psf.beta,
                                             psf.flux,
                                             psf.meanSignal,
                                             psf.mad ) );
         }
      }
   }

   f.Close();
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::DrawStar( VectorGraphics& g, double penWidth, const Star& star,
                                    ImageWindow& window, const Rect& r0 ) const
{
   DRect rect = window.ImageToViewport( star.rect );
   if ( r0.Intersects( rect ) )
   {
      g.SetPen( star.selected ? instance.p_selectedStarColor : instance.p_starColor, penWidth );

      if ( star )
      {
         g.DisableAntialiasing();

         g.StrokeRect( rect );

         g.EnableAntialiasing();

         for ( const PSF& psf : star.psfs )
         {
            DPoint d = window.ImageToViewport( psf.c0 );
            g.TranslateTransformation( d.x, d.y );
            g.RotateTransformation( Rad( psf.theta ) );
            double dr = penWidth * CENTER_RADIUS;
            g.DrawLine( -dr, 0, +dr, 0 );
            g.DrawLine( 0, -dr, 0, +dr );

            double rx = window.ImageScalarToViewport( psf.FWHMx() );
            double ry = window.ImageScalarToViewport( psf.FWHMy() );
            if ( rx > 0.5 && ry > 0.5 )
               g.StrokeEllipse( -rx, -ry, +rx, +ry );

            g.ResetTransformation();
         }
      }
      else
      {
         g.FillRect( rect, instance.p_badStarFillColor );
      }
   }
}

// ----------------------------------------------------------------------------

DynamicPSFInterface::Star* DynamicPSFInterface::StarFromTreeBoxNode( TreeBox::Node& node )
{
   StarNode* starNode = dynamic_cast<StarNode*>( &node );
   if ( starNode != nullptr )
      return starNode->star;
   PSFNode* psfNode = dynamic_cast<PSFNode*>( &node );
   if ( psfNode != nullptr )
      return psfNode->psf->star;
   return nullptr;
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::ExpandTreeBoxNodes( TreeBox::Node* node )
{
   if ( node != nullptr )
   {
      for ( int n = node->NumberOfChildren(), i = n; --i >= 0; )
         ExpandTreeBoxNodes( node->Child( i ) );
      node->Expand();
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::CollapseTreeBoxNodes( TreeBox::Node* node )
{
   if ( node != nullptr )
   {
      for ( int n = node->NumberOfChildren(), i = n; --i >= 0; )
         CollapseTreeBoxNodes( node->Child( i ) );
      node->Collapse();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DynamicPSFInterface::Star::Regenerate( const DynamicPSFInterface::PSFOptions& options )
{
   ImageVariant image = collection->view.Image();
   Regenerate( image, options );
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::Star::Regenerate( const ImageVariant& image, const DynamicPSFInterface::PSFOptions& options )
{
   psfs.Destroy();

   if ( !*this )
      return;

   image.SelectChannel( channel );

   if ( options.autoPSF )
   {
      Array<PSFFit> fits;

      PSFFit Fg( image, pos, rect, PSFunction::Gaussian, options.circular );
      if ( Fg )
         fits.Add( Fg );

      PSFFit Fm( image, pos, rect, PSFunction::Moffat, options.circular );
      if ( Fm )
         fits.Add( Fm );
      else
      {
         /*
          * Moffat fit didn't converge - try all fixed beta functions
          */
         PSFFit FA( image, pos, rect, PSFunction::MoffatA, options.circular );
         if ( FA )
            fits.Add( FA );
         PSFFit F8( image, pos, rect, PSFunction::Moffat8, options.circular );
         if ( F8 )
            fits.Add( F8 );
         PSFFit F6( image, pos, rect, PSFunction::Moffat6, options.circular );
         if ( F6 )
            fits.Add( F6 );
         PSFFit F4( image, pos, rect, PSFunction::Moffat4, options.circular );
         if ( F4 )
            fits.Add( F4 );
         PSFFit F25( image, pos, rect, PSFunction::Moffat25, options.circular );
         if ( F25 )
            fits.Add( F25 );
         PSFFit F15( image, pos, rect, PSFunction::Moffat15, options.circular );
         if ( F15 )
            fits.Add( F15 );
         PSFFit F1( image, pos, rect, PSFunction::Lorentzian, options.circular );
         if ( F1 )
            fits.Add( F1 );
      }

      if ( options.autoVariableShapePSF )
      {
         PSFFit Fv( image, pos, rect, PSFunction::VariableShape, options.circular, options.betaMin, options.betaMax );
         if ( Fv )
            fits.Add( Fv );
      }

      if ( !fits.IsEmpty() )
      {
         PSFFit F = fits[0];
         for ( size_type i = 1; i < fits.Length(); ++i )
            if ( fits[i].psf.mad < F.psf.mad )
               F = fits[i];
         AddPSF( F.psf );
      }
   }
   else
   {
      if ( options.gaussian )
      {
         PSFFit F( image, pos, rect, PSFunction::Gaussian, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.variableShape )
      {
         PSFFit F( image, pos, rect, PSFunction::VariableShape, options.circular, options.betaMin, options.betaMax );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffat )
      {
         PSFFit F( image, pos, rect, PSFunction::Moffat, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffatA )
      {
         PSFFit F( image, pos, rect, PSFunction::MoffatA, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffat8 )
      {
         PSFFit F( image, pos, rect, PSFunction::Moffat8, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffat6 )
      {
         PSFFit F( image, pos, rect, PSFunction::Moffat6, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffat4 )
      {
         PSFFit F( image, pos, rect, PSFunction::Moffat4, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffat25 )
      {
         PSFFit F( image, pos, rect, PSFunction::Moffat25, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffat15 )
      {
         PSFFit F( image, pos, rect, PSFunction::Moffat15, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.lorentzian )
      {
         PSFFit F( image, pos, rect, PSFunction::Lorentzian, options.circular );
         if ( F )
            AddPSF( F.psf );
      }
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::Star::Regenerate( float threshold, bool autoAperture,
                                            const DynamicPSFInterface::PSFOptions& options )
{
   ImageVariant image = collection->view.Image();
   Regenerate( image, threshold, autoAperture, options );
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::Star::Regenerate( const ImageVariant& image, float threshold, bool autoAperture,
                                            const DynamicPSFInterface::PSFOptions& options )
{
   StarDetector D( collection->image, channel, pos, RoundInt( rect.Width()/2 ), threshold, autoAperture );
   AssignData( D.star );
   Regenerate( image, options );
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::Star::Recalculate( float threshold, bool autoAperture,
                                             const DynamicPSFInterface::PSFOptions& options )
{
   ImageVariant image = collection->view.Image();
   Recalculate( image, threshold, autoAperture, options );
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::Star::Recalculate( const ImageVariant& image, float threshold, bool autoAperture,
                                             const DynamicPSFInterface::PSFOptions& options )
{
   StarDetector D( collection->image, channel, pos, RoundInt( rect.Width()/2 ), threshold, autoAperture );
   if ( D )
   {
      AssignData( D.star );
      image.SelectChannel( channel );
      for ( PSF& psf : psfs )
      {
         PSFFit F( image, pos, rect, psf.function, psf.circular, options.betaMin, options.betaMax );
         if ( F )
         {
            psf.AssignData( F.psf );
            psf.celestial = collection->astrometricSolution != nullptr &&
                            collection->astrometricSolution->ImageToCelestial( psf.q0, psf.c0 + collection->offset );
         }
         else
            psf.status = F.psf.status;
      }
   }
   else
   {
      status = D.star.status;
      for ( PSF& psf : psfs )
         psf.status = PSFFitStatus::NotFitted;
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::Star::Update()
{
   if ( !collection->view.IsNull() )
   {
      ImageWindow window = collection->view.Window();
      window.UpdateViewportRect( window.ImageToViewport( rect ).RoundedToInt().InflatedBy( 1 ) );
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::Star::UpdateNodes()
{
   if ( node != nullptr )
   {
      for ( PSF& psf : psfs )
         if ( psf.node != nullptr )
            psf.node->Update();
      node->Update();
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::Star::DestroyPSFNodes()
{
   for ( PSF& psf : psfs )
      delete psf.node;
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::Star::CreatePSFNodes()
{
   if ( node != nullptr )
      for ( PSF& psf : psfs )
         new PSFNode( *node, &psf );
}

// ----------------------------------------------------------------------------

double DynamicPSFInterface::Star::SortingValue( DynamicPSFInterface::SortingCriterion sortBy ) const
{
   if ( sortBy == DynamicPSFInterface::SortByStarId )
      return uniqueId;

   if ( psfs.IsEmpty() )
      return 0;

   switch ( sortBy )
   {
   case DynamicPSFInterface::SortByBackground:
      {
         double B = psfs[0].B;
         for ( const PSF& psf : psfs )
            if ( psf.B < B )
               B = psf.B;
         return B;
      }

   case DynamicPSFInterface::SortByAmplitude:
      {
         double A = psfs[0].A;
         for ( const PSF& psf : psfs )
            if ( psf.A < A )
               A = psf.A;
         return A;
      }

   case DynamicPSFInterface::SortBySigma:
      {
         double s = Max( psfs[0].sx, psfs[0].sy );
         for ( const PSF& psf : psfs )
         {
            double si = Max( psf.sx, psf.sy );
            if ( si < s )
               s = si;
         }
         return s;
      }

   case DynamicPSFInterface::SortByAspectRatio:
      {
         double r = psfs[0].sy/psfs[0].sx;
         for ( const PSF& psf : psfs )
         {
            double ri = psf.sy/psf.sx;
            if ( ri < r )
               r = ri;
         }
         return r;
      }

   case DynamicPSFInterface::SortByRotationAngle:
      {
         double theta = psfs[0].theta;
         if ( *signedAngles )
            if ( theta > 90 )
               theta -= 180;
         for ( const PSF& psf : psfs )
         {
            double itheta = psf.theta;
            if ( *signedAngles )
               if ( itheta > 90 )
                  itheta -= 180;
            if ( itheta < theta )
               theta = itheta;
         }
         return theta;
      }

   case DynamicPSFInterface::SortByAbsRotationAngle:
      {
         double theta = psfs[0].theta;
         for ( const PSF& psf : psfs )
            if ( psf.theta < theta )
               theta = psf.theta;
         return theta;
      }

   case DynamicPSFInterface::SortByShape:
      {
         double beta = 0;
         for ( psf_list::const_iterator i = psfs.Begin(); i != psfs.End(); ++i )
            if ( i->function != PSFunction::Gaussian )
            {
               beta = i->beta;
               for ( psf_list::const_iterator j = i; ++j != psfs.End(); )
                  if ( j->beta < beta )
                     beta = j->beta;
               break;
            }
         return beta;
      }

   case DynamicPSFInterface::SortByFlux:
      {
         double flux = psfs[0].flux;
         for ( const PSF& psf : psfs )
            if ( psf.flux < flux )
               flux = psf.flux;
         return flux;
      }

   case DynamicPSFInterface::SortByMeanSignal:
      {
         double meanSignal = psfs[0].meanSignal;
         for ( const PSF& psf : psfs )
            if ( psf.meanSignal < meanSignal )
               meanSignal = psf.meanSignal;
         return meanSignal;
      }

   default:
   case DynamicPSFInterface::SortByMAD:
      {
         double mad = psfs[0].mad;
         for ( const PSF& psf : psfs )
            if ( psf.mad < mad )
               mad = psf.mad;
         return mad;
      }
   }

   return 0; // ?
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DynamicPSFInterface::PSFCollection::UpdateDetectionImage()
{
   image.FreeData();

   if ( !view.IsNull() )
   {
      ImageVariant v = view.Image();
      ImageVariant( &image ).CopyImage( v );
      image.ResetSelections();
      image.Status().DisableInitialization();

      // Apply a 5x5 Gaussian blur for noise reduction
      static float G5hv[] = { 0.010000, 0.316228, 1.000000, 0.316228, 0.010000 };
      SeparableFilter G5( G5hv, G5hv, 5 );
      SeparableConvolution C( G5 );
      C >> image;
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::PSFCollection::GetAstrometricSolution()
{
   if ( !view.IsNull() )
   {
      ImageWindow window = view.Window();
      if ( window.HasAstrometricSolution() )
         astrometricSolution = new AstrometricMetadata( window );
      if ( view.IsPreview() )
         offset = window.PreviewRect( view.Id() ).LeftTop();
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::PSFCollection::Regenerate( float threshold, bool autoAperture,
                                                     const DynamicPSFInterface::PSFOptions& options )
{
   if ( view.IsNull() )
   {
      for ( Star& star : stars )
      {
         star.status = StarDetector::NotDetected;
         star.psfs.Destroy();
      }
   }
   else
   {
      ImageVariant image = view.Image();
      Array<size_type> L = Thread::OptimalThreadLoads( stars.Length() );
      if ( L.Length() == 1 )
      {
         for ( Star& star : stars )
            star.Regenerate( image, threshold, autoAperture, options );
      }
      else
      {
         ReferenceArray<RegenerateThread> threads;
         for ( size_type i = 0, n = 0; i < L.Length(); n += L[i++] )
            threads.Add( new RegenerateThread( image, threshold, autoAperture, options,
                                               stars.At( n ),
                                               stars.At( n + L[i] ) ) );
         for ( int i = 0; i < int( L.Length() ); ++i )
            threads[i].Start( ThreadPriority::DefaultMax, i );
         for ( int i = 0; i < int( L.Length() ); ++i )
            threads[i].Wait();
         threads.Destroy();
      }
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::PSFCollection::Recalculate( float threshold, bool autoAperture,
                                                      const DynamicPSFInterface::PSFOptions& options )
{
   if ( view.IsNull() )
   {
      for ( Star& star : stars )
      {
         star.status = StarDetector::NotDetected;
         for ( PSF& psf : star.psfs )
            psf.status = PSFFitStatus::NotFitted;
      }
   }
   else
   {
      ImageVariant image = view.Image();
      Array<size_type> L = Thread::OptimalThreadLoads( stars.Length() );
      if ( L.Length() == 1 )
      {
         for ( Star& star : stars )
            star.Recalculate( image, threshold, autoAperture, options );
      }
      else
      {
         ReferenceArray<RecalculateThread> threads;
         for ( size_type i = 0, n = 0; i < L.Length(); n += L[i++] )
            threads.Add( new RecalculateThread( image, threshold, autoAperture, options,
                                                stars.At( n ),
                                                stars.At( n + L[i] ) ) );
         for ( int i = 0; i < int( L.Length() ); ++i )
            threads[i].Start( ThreadPriority::DefaultMax, i );
         for ( int i = 0; i < int( L.Length() ); ++i )
            threads[i].Wait();
         threads.Destroy();
      }
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::PSFCollection::Update()
{
   if ( !view.IsNull() )
   {
      ImageWindow window = view.Window();
      if ( view == window.CurrentView() )
      {
         Rect visibleRect = window.VisibleViewportRect();
         for ( const Star& star : stars )
         {
            Rect rect = window.ImageToViewport( star.rect ).RoundedToInt().InflatedBy( 1 );
            if ( visibleRect.Intersects( rect ) )
               window.UpdateViewportRect( rect );
         }
      }
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::PSFCollection::Update( const Rect& rect )
{
   if ( !view.IsNull() )
   {
      ImageWindow window = view.Window();
      if ( view == window.CurrentView() )
      {
         Rect visibleRect = window.VisibleViewportRect();
         for ( const Star& star : stars )
            if ( rect.Intersects( star.rect ) )
            {
               Rect rect = window.ImageToViewport( star.rect ).RoundedToInt().InflatedBy( 1 );
               if ( visibleRect.Intersects( rect ) )
                  window.UpdateViewportRect( rect );
            }
      }
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::PSFCollection::UpdateImageScale( pcl_enum scaleMode, float scaleValue, const IsoString& scaleKeyword )
{
   xScale = yScale = 0;

   if ( !view.IsNull() )
      switch ( scaleMode )
      {
      default:
      case DPScaleMode::Pixels:
         break;

      case DPScaleMode::StandardMetadata:
         {
            View mainView = view.Window().MainView();
            double focalLength = 0, xPixSize = 0, yPixSize = 0;

            Variant varFocalLength = mainView.PropertyValue( "Instrument:Telescope:FocalLength" );
            Variant varXPixSize = mainView.PropertyValue( "Instrument:Sensor:XPixelSize" );
            Variant varYPixSize = mainView.PropertyValue( "Instrument:Sensor:YPixelSize" );

            if ( varFocalLength.IsValid() && varXPixSize.IsValid() )
            {
               focalLength = varFocalLength.ToDouble() * 1000; // m -> mm
               xPixSize = varXPixSize.ToDouble(); // um
               if ( varYPixSize.IsValid() )
                  yPixSize = varYPixSize.ToDouble(); // um
            }
            else
            {
               FITSKeywordArray keywords = view.Window().Keywords();
               focalLength = KeywordValue( keywords, "FOCALLEN" ); // mm
               xPixSize = KeywordValue( keywords, "XPIXSZ" ); // um
               yPixSize = KeywordValue( keywords, "YPIXSZ" ); // um
            }

            if ( focalLength > 0 )
            {
               if ( yPixSize <= 0 )
                  yPixSize = xPixSize;
               if ( xPixSize > 0 )
               {
                  xScale = 3.6 * Deg( 2*ArcTan( xPixSize, 2*focalLength ) );
                  yScale = 3.6 * Deg( 2*ArcTan( yPixSize, 2*focalLength ) );
               }
            }
         }
         break;

      case DPScaleMode::CustomKeyword:
         {
            FITSKeywordArray keywords;
            view.Window().GetKeywords( keywords );
            xScale = yScale = KeywordValue( keywords, scaleKeyword );
         }
         break;

      case DPScaleMode::LiteralValue:
         xScale = yScale = scaleValue;
         break;
      }

   if ( node != nullptr )
      for ( Star& star : stars )
         if ( star.node != nullptr )
            for ( PSF& psf : star.psfs )
               if ( psf.node != nullptr )
                  psf.node->Update();
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::PSFCollection::UpdateNodes()
{
   if ( node != nullptr )
   {
      for ( Star& star : stars )
         star.UpdateNodes();
      node->Update();
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::PSFCollection::DestroyStarNodes()
{
   for ( Star& star : stars )
   {
      star.DestroyPSFNodes();
      delete star.node;
   }
}

// ----------------------------------------------------------------------------

void DynamicPSFInterface::PSFCollection::CreateStarNodes()
{
   if ( node != nullptr )
      for ( Star& star : stars )
      {
         new StarNode( *node, &star );
         star.CreatePSFNodes();
      }
}

// ----------------------------------------------------------------------------

class StarSortingBinaryPredicate
{
public:

   StarSortingBinaryPredicate( DynamicPSFInterface::SortingCriterion criterion )
      : m_criterion( criterion )
   {
   }

   bool operator ()( const DynamicPSFInterface::Star& s1, const DynamicPSFInterface::Star& s2 ) const
   {
      return s1.SortingValue( m_criterion ) < s2.SortingValue( m_criterion );
   }

private:

   DynamicPSFInterface::SortingCriterion m_criterion;
};

void DynamicPSFInterface::PSFCollection::Sort( DynamicPSFInterface::SortingCriterion sortBy )
{
   stars.Sort( StarSortingBinaryPredicate( sortBy ) );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DynamicPSFInterface::GUIData::GUIData( DynamicPSFInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Image scale (arcsec/px):" ) + 'T' );
   int editWidth1 = fnt.Width( String( '0', 20 ) );

   //

   Data_TreeBox.SetMinHeight( DATATREE_MINHEIGHT( w.Font() ) );
   Data_TreeBox.SetNumberOfColumns( 19 );
   Data_TreeBox.SetHeaderText(  0, "" );   // icon/view-id/star-no/PSF-func
   Data_TreeBox.SetHeaderText(  1, "Ch" );
   Data_TreeBox.SetHeaderText(  2, "B" );
   Data_TreeBox.SetHeaderText(  3, "A" );
   Data_TreeBox.SetHeaderText(  4, "cx" );
   Data_TreeBox.SetHeaderText(  5, "cy" );
   Data_TreeBox.SetHeaderText(  6, u"\u03B1" );  // alpha
   Data_TreeBox.SetHeaderText(  7, u"\u03B4" );  // delta
   Data_TreeBox.SetHeaderText(  8, u"\u03C3x" ); // sigmax
   Data_TreeBox.SetHeaderText(  9, u"\u03C3y" ); // sigmay
   Data_TreeBox.SetHeaderText( 10, "FWHMx" );
   Data_TreeBox.SetHeaderText( 11, "FWHMy" );
   Data_TreeBox.SetHeaderText( 12, "r" );
   Data_TreeBox.SetHeaderText( 13, u"\u03B8" );  // theta
   Data_TreeBox.SetHeaderText( 14, u"\u03B2" );  // beta
   Data_TreeBox.SetHeaderText( 15, "flux" );
   Data_TreeBox.SetHeaderText( 16, "signal" );
   Data_TreeBox.SetHeaderText( 17, "MAD" );
   Data_TreeBox.SetHeaderText( 18, String() ); // stretch
   for ( int i = 0; i < Data_TreeBox.NumberOfColumns(); ++i )
      Data_TreeBox.SetHeaderAlignment( i, TextAlign::Center|TextAlign::VertCenter );
   Data_TreeBox.EnableRootDecoration();
   Data_TreeBox.EnableAlternateRowColor();
   Data_TreeBox.EnableMultipleSelections();

   Data_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&DynamicPSFInterface::__CurrentNodeUpdated, w );
   Data_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&DynamicPSFInterface::__NodeActivated, w );
   Data_TreeBox.OnNodeDoubleClicked( (TreeBox::node_event_handler)&DynamicPSFInterface::__NodeDoubleClicked, w );
   Data_TreeBox.OnNodeExpanded( (TreeBox::node_expand_event_handler)&DynamicPSFInterface::__NodeExpanded, w );
   Data_TreeBox.OnNodeCollapsed( (TreeBox::node_expand_event_handler)&DynamicPSFInterface::__NodeCollapsed, w );
   Data_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&DynamicPSFInterface::__NodeSelectionUpdated, w );

   //

   StarInfo_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   StarInfo_Label.SetStyle( FrameStyle::Sunken );
   StarInfo_Label.SetLineWidth( 1 );
   StarInfo_Label.SetMinWidth( fnt.Width( String( '0', 60 ) ) );

   EnlargeFont_ToolButton.SetIcon( w.ScaledResource( ":/icons/font-grow.png" ) );
   EnlargeFont_ToolButton.SetScaledFixedSize( 24, 24 );
   EnlargeFont_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   EnlargeFont_ToolButton.SetToolTip( "<p>Enlarge font.</p>" );
   EnlargeFont_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   ShrinkFont_ToolButton.SetIcon( w.ScaledResource( ":/icons/font-shrink.png" ) );
   ShrinkFont_ToolButton.SetScaledFixedSize( 24, 24 );
   ShrinkFont_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ShrinkFont_ToolButton.SetToolTip( "<p>Shrink font.</p>" );
   ShrinkFont_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   ExpandAll_ToolButton.SetIcon( w.ScaledResource( ":/browser/expand.png" ) );
   ExpandAll_ToolButton.SetScaledFixedSize( 24, 24 );
   ExpandAll_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExpandAll_ToolButton.SetToolTip( "<p>Expand all tree nodes.</p>" );
   ExpandAll_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   CollapseAll_ToolButton.SetIcon( w.ScaledResource( ":/browser/collapse.png" ) );
   CollapseAll_ToolButton.SetScaledFixedSize( 24, 24 );
   CollapseAll_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   CollapseAll_ToolButton.SetToolTip( "<p>Collapse selected or all tree nodes.</p>" );
   CollapseAll_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   DeleteStar_ToolButton.SetIcon( w.ScaledResource( ":/icons/remove.png" ) );
   DeleteStar_ToolButton.SetScaledFixedSize( 24, 24 );
   DeleteStar_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   DeleteStar_ToolButton.SetToolTip( "<p>Delete selected item(s).</p>" );
   DeleteStar_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   TrackStars_ToolButton.SetIcon( w.ScaledResource( ":/icons/goto-next.png" ) );
   TrackStars_ToolButton.SetScaledFixedSize( 24, 24 );
   TrackStars_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   TrackStars_ToolButton.SetToolTip( "<p>Track star positions on source views.</p>" );
   TrackStars_ToolButton.SetCheckable();
   TrackStars_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Regenerate_ToolButton.SetIcon( w.ScaledResource( ":/icons/redo.png" ) );
   Regenerate_ToolButton.SetScaledFixedSize( 24, 24 );
   Regenerate_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Regenerate_ToolButton.SetToolTip( "<p>Regenerate the selected star(s) with the current fitting parameters.</p>" );
   Regenerate_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   RegenerateAll_ToolButton.SetIcon( w.ScaledResource( ":/icons/reload.png" ) );
   RegenerateAll_ToolButton.SetScaledFixedSize( 24, 24 );
   RegenerateAll_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   RegenerateAll_ToolButton.SetToolTip( "<p>Regenerate all stars with the current fitting parameters.</p>" );
   RegenerateAll_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Sort_ToolButton.SetIcon( w.ScaledResource( ":/icons/sort-down.png" ) );
   Sort_ToolButton.SetScaledFixedSize( 24, 24 );
   Sort_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Sort_ToolButton.SetToolTip( "<p>Sort the list of stars by selectable criteria.</p>" );
   Sort_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   ExportPSF_ToolButton.SetIcon( w.ScaledResource( ":/icons/camera.png" ) );
   ExportPSF_ToolButton.SetScaledFixedSize( 24, 24 );
   ExportPSF_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExportPSF_ToolButton.SetToolTip( "<p>Export a synthetic PSF function as a new image window. The synthetic PSF "
      "will be computed as the normalized average of the fitted functions for the selected star(s).</p>" );
   ExportPSF_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   AverageStars_ToolButton.SetIcon( w.ScaledResource( ":/icons/document-math.png" ) );
   AverageStars_ToolButton.SetScaledFixedSize( 24, 24 );
   AverageStars_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   AverageStars_ToolButton.SetToolTip( "<p>Average PSF parameters for selected stars.</p>" );
   AverageStars_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   ExportCSV_ToolButton.SetIcon( w.ScaledResource( ":/icons/document-text-export.png" ) );
   ExportCSV_ToolButton.SetScaledFixedSize( 24, 24 );
   ExportCSV_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExportCSV_ToolButton.SetToolTip( "<p>Export data in Comma Separated Value (CSV) file format.</p>" );
   ExportCSV_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   StarTools_Sizer.SetSpacing( 4 );
   StarTools_Sizer.Add( StarInfo_Label, 100 );
   StarTools_Sizer.AddSpacing( 12 );
   StarTools_Sizer.Add( EnlargeFont_ToolButton );
   StarTools_Sizer.Add( ShrinkFont_ToolButton );
   StarTools_Sizer.Add( ExpandAll_ToolButton );
   StarTools_Sizer.Add( CollapseAll_ToolButton );
   StarTools_Sizer.AddSpacing( 12 );
   StarTools_Sizer.Add( DeleteStar_ToolButton );
   StarTools_Sizer.AddSpacing( 12 );
   StarTools_Sizer.Add( TrackStars_ToolButton );
   StarTools_Sizer.AddSpacing( 12 );
   StarTools_Sizer.Add( Regenerate_ToolButton );
   StarTools_Sizer.Add( RegenerateAll_ToolButton );
   StarTools_Sizer.Add( Sort_ToolButton );
   StarTools_Sizer.AddSpacing( 12 );
   StarTools_Sizer.Add( ExportPSF_ToolButton );
   StarTools_Sizer.Add( AverageStars_ToolButton );
   StarTools_Sizer.Add( ExportCSV_ToolButton );

   //

   PSFModels_SectionBar.SetTitle( "PSF Models" );
   PSFModels_SectionBar.SetSection( PSFModels_Control );
   PSFModels_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DynamicPSFInterface::__ToggleSection, w );

   AutoPSF_CheckBox.SetText( "Auto" );
   AutoPSF_CheckBox.SetToolTip( "<p>Find the PSF model function that best approximates actual image data.</p>"
      "<p>Only Gaussian and Moffat model functions are used by default. If you enable the <i>Automatic VarShape fits</i> "
      "option, variable shape model functions will also be fitted and selected when they minimize residuals among "
      "PSF models and sampled image data.</p>" );
   AutoPSF_CheckBox.SetChecked();
   AutoPSF_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Gaussian_CheckBox.SetText( "Gaussian" );
   Gaussian_CheckBox.SetToolTip( "<p>A Gaussian PSF model function.</p>" );
   Gaussian_CheckBox.SetChecked();
   Gaussian_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   VariableShape_CheckBox.SetText( "VarShape" );
   VariableShape_CheckBox.SetToolTip( "<p>A variable shape PSF model function.</p>"
      "<p>The <i>beta</i> parameter determines the fitted PSF profile:</p>"
      "<ul>"
      "<li>beta = 2: Gaussian.</li>"
      "<li>beta &lt; 2: leptokurtic (peaked).</li>"
      "<li>beta &gt; 2: platykurtic (flat).</li>"
      "</ul>" );
   VariableShape_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Moffat_CheckBox.SetText( "Moffat" );
   Moffat_CheckBox.SetToolTip( "<p>A Moffat PSF model function with variable beta exponent.</p>" );
   Moffat_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   MoffatA_CheckBox.SetText( "Moffat10" );
   MoffatA_CheckBox.SetToolTip( "<p>A Moffat PSF model function with fixed beta=10</p>" );
   MoffatA_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Moffat8_CheckBox.SetText( "Moffat8" );
   Moffat8_CheckBox.SetToolTip( "<p>A Moffat PSF model function with fixed beta=8</p>" );
   Moffat8_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Moffat6_CheckBox.SetText( "Moffat6" );
   Moffat6_CheckBox.SetToolTip( "<p>A Moffat PSF model function with fixed beta=6</p>" );
   Moffat6_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Moffat4_CheckBox.SetText( "Moffat4" );
   Moffat4_CheckBox.SetToolTip( "<p>A Moffat PSF model function with fixed beta=4</p>" );
   Moffat4_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Moffat25_CheckBox.SetText( "Moffat25" );
   Moffat25_CheckBox.SetToolTip( "<p>A Moffat PSF model function with fixed beta=2.5</p>" );
   Moffat25_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Moffat15_CheckBox.SetText( "Moffat15" );
   Moffat15_CheckBox.SetToolTip( "<p>A Moffat PSF model function with fixed beta=1.5</p>" );
   Moffat15_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Lorentzian_CheckBox.SetText( "Lorentzian" );
   Lorentzian_CheckBox.SetToolTip( "<p>A Lorentzian PSF model function. This is equivalent to a Moffat function with fixed beta=1.</p>" );
   Lorentzian_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   CircularPSF_CheckBox.SetText( "Circular PSF" );
   CircularPSF_CheckBox.SetToolTip( "<p>Enable this option to fit circular PSF functions. Disable it to fit elliptical "
      "functions. An elliptical function provides two separate axes and a rotation angle.</p>"
      "<p>In general, elliptical functions are preferable because they provide more information about the true shapes "
      "and orientations of the fitted PSFs. Sometimes, however, circular functions may be preferable. For example, strongly "
      "undersampled images usually don't provide enough data to fit elliptical functions reliably. Uncertainty due to high "
      "noise levels also affect complex function fittings adversely. In such cases a circular function can provide more "
      "robust results.</p>"
      "<p>Elliptical PSF functions are fitted by default.</p>" );
   CircularPSF_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   SignedAngles_CheckBox.SetText( "Signed angles" );
   SignedAngles_CheckBox.SetToolTip( "<p>When this option is enabled, rotation angles are shown in the [-90&deg;,+90&deg;] "
      "range. When this option is disabled, rotation angles are shown in the [0&deg;,180&deg;) range.</p>"
      "<p>Signed angles are useful to prevent ambiguities introduced by small rotations around zero degrees. These "
      "'oscillatory' rotations occur frequently due to uncertainty in fitting rotation angles for nearly circular stars. "
      "For example, imagine a set of PSF fits where we have two subsets with rotations around zero and 180 degrees. If we "
      "compute average PSF parameters, the resulting average rotation would be around 90 degrees. This may be incorrect if "
      "both subsets are actually due to dispersion caused by fitting uncertainty, in which case the correct average rotation "
      "would be a value close to zero degrees.</p>"
      "<p>This option is enabled by default, so rotation angles are represented as signed quantities in the "
      "(-90&deg;,+90&deg;] range.</p>" );
   SignedAngles_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   PSFModels1_Sizer.SetSpacing( 4 );
   PSFModels1_Sizer.Add( AutoPSF_CheckBox );
   PSFModels1_Sizer.Add( MoffatA_CheckBox );
   PSFModels1_Sizer.Add( CircularPSF_CheckBox );
   PSFModels1_Sizer.AddStretch();

   PSFModels2_Sizer.SetSpacing( 4 );
   PSFModels2_Sizer.Add( Gaussian_CheckBox );
   PSFModels2_Sizer.Add( Moffat8_CheckBox );
   PSFModels2_Sizer.Add( SignedAngles_CheckBox );
   PSFModels2_Sizer.AddStretch();

   PSFModels3_Sizer.SetSpacing( 4 );
   PSFModels3_Sizer.Add( VariableShape_CheckBox );
   PSFModels3_Sizer.Add( Moffat6_CheckBox );
   PSFModels3_Sizer.AddStretch();

   PSFModels4_Sizer.SetSpacing( 4 );
   PSFModels4_Sizer.Add( Moffat_CheckBox );
   PSFModels4_Sizer.Add( Moffat4_CheckBox );
   PSFModels4_Sizer.AddStretch();

   PSFModels5_Sizer.SetSpacing( 4 );
   PSFModels5_Sizer.AddStretch();
   PSFModels5_Sizer.Add( Moffat25_CheckBox );
   PSFModels5_Sizer.AddStretch();

   PSFModels6_Sizer.SetSpacing( 4 );
   PSFModels6_Sizer.AddStretch();
   PSFModels6_Sizer.Add( Moffat15_CheckBox );
   PSFModels6_Sizer.AddStretch();

   PSFModels7_Sizer.SetSpacing( 4 );
   PSFModels7_Sizer.AddStretch();
   PSFModels7_Sizer.Add( Lorentzian_CheckBox );
   PSFModels7_Sizer.AddStretch();

   PSFModelsTop_Sizer.SetSpacing( 16 );
   PSFModelsTop_Sizer.Add( PSFModels1_Sizer );
   PSFModelsTop_Sizer.Add( PSFModels2_Sizer );
   PSFModelsTop_Sizer.Add( PSFModels3_Sizer );
   PSFModelsTop_Sizer.Add( PSFModels4_Sizer );
   PSFModelsTop_Sizer.Add( PSFModels5_Sizer );
   PSFModelsTop_Sizer.Add( PSFModels6_Sizer );
   PSFModelsTop_Sizer.Add( PSFModels7_Sizer );
   PSFModelsTop_Sizer.AddStretch();

   AutoVariableShapePSF_CheckBox.SetText( "Automatic VarShape fits" );
   AutoVariableShapePSF_CheckBox.SetToolTip( "<p>When the Auto PSF model is selected, include variable shape functions to search "
      "for optimal PSF fits. If this option is disabled, only Gaussian and Moffat functions will be used when Auto is selected.</p>"
      "<p>In many cases, variable shape model functions will lead to optimal PSF fits in terms of minimization of absolute deviation "
      "between fitted point spread functions and source image pixel samples for each selected object. Since variable shape PSF models "
      "are nonstandard and relatively experimental, they are not included in automatic optimal fits by default.</p>" );
   AutoVariableShapePSF_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   AutoVariableShapePSF_Sizer.Add( AutoVariableShapePSF_CheckBox );
   AutoVariableShapePSF_Sizer.AddStretch();

   PSFModels_Sizer.SetMargin( 4 );
   PSFModels_Sizer.SetSpacing( 4 );
   PSFModels_Sizer.Add( PSFModelsTop_Sizer );
   PSFModels_Sizer.Add( AutoVariableShapePSF_Sizer );

   PSFModels_Control.SetSizer( PSFModels_Sizer );

   //

   StarDetectionParameters_SectionBar.SetTitle( "Star Detection" );
   StarDetectionParameters_SectionBar.SetSection( StarDetectionParameters_Control );
   StarDetectionParameters_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DynamicPSFInterface::__ToggleSection, w );

   static const char* searchRadiusToolTip =
   "<p>This parameter determines the size in pixels of the initial search box used to detect stars when you clic on "
   "the image. Increase it to favor detection of larger stars. Decrease it to facilitate selection of smaller stars. "
   "For example, a smaller search radius may be necessary to deal with dense star fields.</p>"
   "<p>The default value of eight pixels is quite appropriate in most cases.</p>";

   SearchRadius_Label.SetText( "Search radius:" );
   SearchRadius_Label.SetMinWidth( labelWidth1 );
   SearchRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SearchRadius_Label.SetToolTip( searchRadiusToolTip );

   SearchRadius_SpinBox.SetRange( int( TheDPSearchRadiusParameter->MinimumValue() ),
                                  int( TheDPSearchRadiusParameter->MaximumValue() ) );
   SearchRadius_SpinBox.SetToolTip( searchRadiusToolTip );
   SearchRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&DynamicPSFInterface::__SpinBox_ValueUpdated, w );

   SearchRadius_Sizer.SetSpacing( 4 );
   SearchRadius_Sizer.Add( SearchRadius_Label );
   SearchRadius_Sizer.Add( SearchRadius_SpinBox );
   SearchRadius_Sizer.AddStretch();

   Threshold_NumericEdit.SetReal();
   Threshold_NumericEdit.SetPrecision( TheDPThresholdParameter->Precision() );
   Threshold_NumericEdit.SetRange( TheDPThresholdParameter->MinimumValue(), TheDPThresholdParameter->MaximumValue() );
   Threshold_NumericEdit.label.SetText( "Background threshold:" );
   Threshold_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Threshold_NumericEdit.sizer.AddStretch();
   Threshold_NumericEdit.SetToolTip( "<p>Threshold value for rejection of background pixels, in sigma units.</p>"
      "<p>This value is used by the object detection routine to classify sampled pixels into two disjoint sets: "
      "pixels belonging to the local background, and pixels belonging to the object being detected. A smaller "
      "threshold value makes the star detection routine less sensitive. This can be useful to avoid detection of "
      "very faint objects. On the other hand, a higher threshold allows you to isolate very small and faint objects, "
      "which may or may not be desirable, depending on what you want to do.</p>"
      "<p>The default value of one sigma is normally appropriate in most cases.</p>" );
   Threshold_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicPSFInterface::__NumericEdit_ValueUpdated, w );

   AutoAperture_CheckBox.SetText( "Automatic aperture" );
   AutoAperture_CheckBox.SetToolTip( "<p>Compute optimal sampling dimensions adaptively for each fitted star.</p>"
      "<p>Each star is fitted to a PSF function inside a rectangular <i>sampling region</i>. If the sampling region "
      "is too small, the local background cannot be determined accurately, and hence the PSF fit will be less accurate. "
      "If the sampling region is too large, pixels from nearby objects can contaminate the sample, and computation "
      "times grow unnecessarily. The automatic aperture feature of DynamicPSF implements a robust algorithm to find "
      "the smallest sampling region necessary to minimize uncertainty in the evaluation of the local background.</p>"
      "<p>This option is enabled by default. Disabling it is not recommended under normal working conditions.</p>" );
   AutoAperture_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   AutoAperture_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   AutoAperture_Sizer.Add( AutoAperture_CheckBox );
   AutoAperture_Sizer.AddStretch();

   StarDetectionParameters_Sizer.SetSpacing( 4 );
   StarDetectionParameters_Sizer.Add( SearchRadius_Sizer );
   StarDetectionParameters_Sizer.Add( Threshold_NumericEdit );
   StarDetectionParameters_Sizer.Add( AutoAperture_Sizer );
   StarDetectionParameters_Sizer.AddStretch();

   StarDetectionParameters_Control.SetSizer( StarDetectionParameters_Sizer );

   //

   Scale_SectionBar.SetTitle( "Image Scale" );
   Scale_SectionBar.SetSection( Scale_Control );
   Scale_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DynamicPSFInterface::__ToggleSection, w );

   const char* scaleModeTip = "<p>This parameter tells DynamicPSF how to compute the image scale to show "
      "full width at half maximum (FWHM) values:</p>"
      "<p><b>Pixels</b> will ignore image scale and show FWHM in pixel units. This is the default option.</p>"
      "<p><b>Standard metadata</b> will try to use the following XISF standard properties read from the "
      "target image to compute the scale of the image in arcseconds per pixel:</p>"
      "<ul>"
      "<li>Instrument:Telescope:FocalLength</li>"
      "<li>Instrument:Sensor:XPixelSize</li>"
      "<li>Instrument:Sensor:YPixelSize</li>"
      "</ul>"
      "<p>If these properties are not available, then the following FITS header keywords will be used, if available: "
      "FOCALLEN, XPIXSZ, and YPIXSZ.</p>"
      "<p><b>literal value</b> allows you to specify directly the image scale in arcseconds per pixel.</p>"
      "<p><b>Custom FITS keyword</b> allows you to specify the name of a custom FITS keyword whose value is "
      "the image scale in arcseconds per pixel.</p>";

   ScaleMode_Label.SetText( "Scale mode:" );
   ScaleMode_Label.SetToolTip( scaleModeTip );
   ScaleMode_Label.SetMinWidth( labelWidth1 );
   ScaleMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ScaleMode_ComboBox.AddItem( "Pixels" );
   ScaleMode_ComboBox.AddItem( "Standard metadata" );
   ScaleMode_ComboBox.AddItem( "Literal value" );
   ScaleMode_ComboBox.AddItem( "Custom FITS keyword" );
   ScaleMode_ComboBox.SetToolTip( scaleModeTip );
   ScaleMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DynamicPSFInterface::__ItemSelected, w );

   ScaleMode_Sizer.SetSpacing( 4 );
   ScaleMode_Sizer.Add( ScaleMode_Label );
   ScaleMode_Sizer.Add( ScaleMode_ComboBox );
   ScaleMode_Sizer.AddStretch();

   const char* scaleValueTip = "<p>When <i>scale mode</i> has been set to <i>literal value</i>, this is the image "
      "scale in arcseconds per pixel.</p>";

   ScaleValue_NumericEdit.label.SetText( "Image scale (arcsec/px):" );
   ScaleValue_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ScaleValue_NumericEdit.SetReal();
   ScaleValue_NumericEdit.SetRange( TheDPScaleValueParameter->MinimumValue(), TheDPScaleValueParameter->MaximumValue() );
   ScaleValue_NumericEdit.SetPrecision( TheDPScaleValueParameter->Precision() );
   ScaleValue_NumericEdit.sizer.AddStretch();
   ScaleValue_NumericEdit.SetToolTip( scaleValueTip );
   ScaleValue_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicPSFInterface::__NumericEdit_ValueUpdated, w );

   const char* scaleKeywordTip = "<p>When <i>scale mode</i> has been set to <i>custom FITS keyword</i>, this is "
      "the name of a FITS header keyword whose value is the image scale in arcseconds per pixel.</p>";

   ScaleKeyword_Label.SetText( "Custom keyword:" );
   ScaleKeyword_Label.SetFixedWidth( labelWidth1 );
   ScaleKeyword_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ScaleKeyword_Label.SetToolTip( scaleKeywordTip );

   ScaleKeyword_Edit.SetMinWidth( editWidth1 );
   ScaleKeyword_Edit.SetToolTip( scaleKeywordTip );
   ScaleKeyword_Edit.OnEditCompleted( (Edit::edit_event_handler)&DynamicPSFInterface::__EditCompleted, w );

   ScaleKeyword_Sizer.SetSpacing( 4 );
   ScaleKeyword_Sizer.Add( ScaleKeyword_Label );
   ScaleKeyword_Sizer.Add( ScaleKeyword_Edit );
   ScaleKeyword_Sizer.AddStretch();

   Scale_Sizer.SetSpacing( 4 );
   Scale_Sizer.Add( ScaleMode_Sizer );
   Scale_Sizer.Add( ScaleValue_NumericEdit );
   Scale_Sizer.Add( ScaleKeyword_Sizer );

   Scale_Control.SetSizer( Scale_Sizer );

   //

   Global_Sizer.SetMargin( 6 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Data_TreeBox, 100 );
   Global_Sizer.Add( StarTools_Sizer );
   Global_Sizer.Add( PSFModels_SectionBar );
   Global_Sizer.Add( PSFModels_Control );
   Global_Sizer.Add( StarDetectionParameters_SectionBar );
   Global_Sizer.Add( StarDetectionParameters_Control );
   Global_Sizer.Add( Scale_SectionBar );
   Global_Sizer.Add( Scale_Control );

   w.SetSizer( Global_Sizer );

   StarDetectionParameters_Control.Hide();
   Scale_Control.Hide();

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DynamicPSFInterface.cpp - Released 2021-04-09T19:41:48Z
