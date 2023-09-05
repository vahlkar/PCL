//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.5.8
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.4
// ----------------------------------------------------------------------------
// DynamicPSFInterface.h - Released 2023-08-28T15:23:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#ifndef __DynamicPSFInterface_h
#define __DynamicPSFInterface_h

#include <pcl/AstrometricMetadata.h>
#include <pcl/Atomic.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Graphics.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/ReferenceArray.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/Thread.h>
#include <pcl/TreeBox.h>
#include <pcl/View.h>

#include "DynamicPSFInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class Graphics;
class ImageWindow;

class PSFCollectionNode;
class StarNode;
class PSFNode;

// ----------------------------------------------------------------------------

class DynamicPSFInterface : public ProcessInterface
{
public:

   DynamicPSFInterface();
   virtual ~DynamicPSFInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool IsDynamicInterface() const override;
   void ExitDynamicMode() override;
   void DynamicMouseEnter( View& ) override;
   void DynamicMouseLeave( View& ) override;
   void DynamicMouseMove( View&, const DPoint&, unsigned buttons, unsigned modifiers ) override;
   void DynamicMousePress( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers ) override;
   void DynamicMouseRelease( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers ) override;
   void DynamicMouseDoubleClick( View&, const DPoint&, unsigned buttons, unsigned modifiers ) override;
   bool DynamicKeyPress( View& v, int key, unsigned modifiers );
   bool RequiresDynamicUpdate( const View&, const DRect& ) const override;
   void DynamicPaint( const View&, VectorGraphics&, const DRect& ) const override;
   void SaveSettings() const override;
   void LoadSettings() override;

   static void ExecuteInstance( DynamicPSFInstance& );

   int AstrometryPrecision() const
   {
      return m_instance.p_astrometryPrecision;
   }

private:

   DynamicPSFInstance m_instance;

   bool m_showFWHM = true;
   bool m_showFWTM = true;
   bool m_showBestFit = true;

   struct GUIData
   {
      GUIData( DynamicPSFInterface& );

      VerticalSizer  Global_Sizer;
         TreeBox           Data_TreeBox;
         HorizontalSizer   StarTools_Sizer;
            Label             StarInfo_Label;
            ToolButton        EnlargeFont_ToolButton;
            ToolButton        ShrinkFont_ToolButton;
            ToolButton        ExpandAll_ToolButton;
            ToolButton        CollapseAll_ToolButton;
            ToolButton        DeleteStar_ToolButton;
            ToolButton        TrackStars_ToolButton;
            ToolButton        Regenerate_ToolButton;
            ToolButton        RegenerateAll_ToolButton;
            ToolButton        Sort_ToolButton;
            ToolButton        ExportPSF_ToolButton;
            ToolButton        AverageStars_ToolButton;
            ToolButton        ExportCSV_ToolButton;
         SectionBar        PSFModels_SectionBar;
         Control           PSFModels_Control;
            VerticalSizer     PSFModels_Sizer;
               HorizontalSizer   PSFModelsTop_Sizer;
                  VerticalSizer     PSFModels1_Sizer;
                     CheckBox          AutoPSF_CheckBox;
                     CheckBox          MoffatA_CheckBox;
                     CheckBox          CircularPSF_CheckBox;
                  VerticalSizer     PSFModels2_Sizer;
                     CheckBox          Gaussian_CheckBox;
                     CheckBox          Moffat8_CheckBox;
                     CheckBox          SignedAngles_CheckBox;
                  VerticalSizer     PSFModels3_Sizer;
                     CheckBox          VariableShape_CheckBox;
                     CheckBox          Moffat6_CheckBox;
                  VerticalSizer     PSFModels4_Sizer;
                     CheckBox          Moffat_CheckBox;
                     CheckBox          Moffat4_CheckBox;
                  VerticalSizer     PSFModels5_Sizer;
                     CheckBox          Moffat25_CheckBox;
                  VerticalSizer     PSFModels6_Sizer;
                     CheckBox          Moffat15_CheckBox;
                  VerticalSizer     PSFModels7_Sizer;
                     CheckBox          Lorentzian_CheckBox;
               HorizontalSizer   AutoVariableShapePSF_Sizer;
                  CheckBox          AutoVariableShapePSF_CheckBox;
               HorizontalSizer   DrawEllipses_Sizer;
                  CheckBox          ShowFWHM_CheckBox;
                  CheckBox          ShowFWTM_CheckBox;
                  CheckBox          ShowBestFit_CheckBox;

         SectionBar        StarDetectionParameters_SectionBar;
         Control           StarDetectionParameters_Control;
         VerticalSizer     StarDetectionParameters_Sizer;
            HorizontalSizer   SearchRadius_Sizer;
               Label             SearchRadius_Label;
               SpinBox           SearchRadius_SpinBox;
            NumericEdit       Threshold_NumericEdit;
            HorizontalSizer   AutoAperture_Sizer;
               CheckBox          AutoAperture_CheckBox;
         SectionBar        Scale_SectionBar;
         Control           Scale_Control;
         VerticalSizer     Scale_Sizer;
            HorizontalSizer   ScaleMode_Sizer;
               Label             ScaleMode_Label;
               ComboBox          ScaleMode_ComboBox;
            NumericEdit       ScaleValue_NumericEdit;
            HorizontalSizer   ScaleKeyword_Sizer;
               Label             ScaleKeyword_Label;
               Edit              ScaleKeyword_Edit;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateStarInfo();
   void UpdateScaleItems();
   void UpdateStyleSheets();

   void RegenerateDataTree();
   void AdjustDataTreeColumns();

   void SetPSFunction( PSFData::psf_function, bool );

   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void e_NodeDoubleClicked( TreeBox& sender, TreeBox::Node& node, int col );
   void e_NodeExpanded( TreeBox& sender, TreeBox::Node& node );
   void e_NodeCollapsed( TreeBox& sender, TreeBox::Node& node );
   void e_NodeSelectionUpdated( TreeBox& sender );
   void e_Click( Button& sender, bool checked );
   void e_NumericEdit_ValueUpdated( NumericEdit& sender, double value );
   void e_SpinBox_ValueUpdated( SpinBox& sender, int value );
   void e_ToggleSection( SectionBar& sender, Control& section, bool start );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_EditCompleted( Edit& sender );

   // -------------------------------------------------------------------------

   typedef DynamicPSFInstance::PSFOptions PSFOptions;

   enum SortingCriterion
   {
      SortByStarId,
      SortByBackground,
      SortByAmplitude,
      SortBySigma,
      SortByAspectRatio,
      SortByRotationAngle,
      SortByAbsRotationAngle,
      SortByShape,
      SortByFlux,
      SortByMeanSignal,
      SortByMAD
   };

   struct Star;
   struct PSFCollection;

   // -------------------------------------------------------------------------

   struct PSF : public PSFData
   {
      Star*    star = nullptr; // star being fitted
      PSFNode* node = nullptr;

      PSF( const PSFData& data, Star* s )
         : PSFData( data )
         , star( s )
      {
      }

      virtual ~PSF();

      void AssignData( const PSFData& data )
      {
         *static_cast<PSFData*>( this ) = data;
      }
   };

   // -------------------------------------------------------------------------

   typedef ReferenceArray<PSF>  psf_list;

   struct Star : public StarData
   {
      typedef DynamicPSFInterface::psf_list psf_list;

      PSFCollection* collection = nullptr; // the view to which this star pertains
      psf_list       psfs;                 // fitted PSFs
      StarNode*      node = nullptr;
      unsigned       uniqueId = 0;
      bool           selected = false;

      Star( PSFCollection* c )
         : collection( c )
         , uniqueId( c->UniqueStarId() )
      {
      }

      Star( const StarData& data, PSFCollection* c )
         : StarData( data )
         , collection( c )
         , uniqueId( c->UniqueStarId() )
      {
      }

      virtual ~Star();

      void AssignData( const StarData& data )
      {
         *static_cast<StarData*>( this ) = data;
      }

      PSF* AddPSF( const PSFData& data )
      {
         PSF* psf = new PSF( data, this );
         psf->celestial = collection->astrometricSolution != nullptr &&
                          collection->astrometricSolution->ImageToCelestial( psf->q0, psf->c0 + collection->offset );
         psfs << psf;
         rect |= psf->Bounds();
         return psf;
      }

      void Regenerate( const PSFOptions& options );
      void Regenerate( const ImageVariant&, const PSFOptions& options );

      void Regenerate( float threshold, bool autoAperture, const PSFOptions& options );
      void Regenerate( const ImageVariant&, float threshold, bool autoAperture, const PSFOptions& options );

      void Recalculate( float threshold, bool autoAperture, const PSFOptions& options );
      void Recalculate( const ImageVariant&, float threshold, bool autoAperture, const PSFOptions& options );

      void Update();

      void UpdateNodes();
      void DestroyPSFNodes();
      void CreatePSFNodes();

      double SortingValue( SortingCriterion ) const;
   };

   typedef ReferenceArray<Star> star_list;

   struct PSFCollection
   {
      typedef DynamicPSFInterface::star_list star_list;

      View               view;
      IsoString          viewId; // required in case view.IsNull()
      AutoPointer<AstrometricMetadata>
                         astrometricSolution;
      DPoint             offset = 0.0;
      float              xScale = 0; // image scale in arcsec/px
      float              yScale = 0;
      Image              image;  // detection image, preprocessed.
      star_list          stars;
      PSFCollectionNode* node = nullptr;

      PSFCollection( const IsoString& id, bool astrometry = true )
         : view( View::ViewById( id ) )
         , viewId( id )
      {
         if ( !view.IsNull() )
         {
            view.AddToDynamicTargets();
            UpdateDetectionImage();
            if ( astrometry )
               GetAstrometricSolution();
         }
      }

      template <class S> PSFCollection( const S& id, bool astrometry = true )
         : PSFCollection( IsoString( id ), astrometry )
      {
      }

      PSFCollection( const View& v, bool astrometry = true )
         : view( v )
         , viewId( v.FullId() )
      {
         if ( !view.IsNull() )
         {
            view.AddToDynamicTargets();
            UpdateDetectionImage();
            if ( astrometry )
               GetAstrometricSolution();
         }
      }

      virtual ~PSFCollection();

      IsoString ViewId() const
      {
         return view.IsNull() ? viewId : view.FullId();
      }

      Star* AddStar( const StarData& data )
      {
         Star* star = new Star( data, this );
         stars.Add( star );
         return star;
      }

      void Regenerate( float threshold, bool autoAperture, const PSFOptions& options );
      void Recalculate( float threshold, bool autoAperture, const PSFOptions& options );

      void Update();
      void Update( const Rect& );

      void UpdateImageScale( pcl_enum scaleMode, float scaleValue, const IsoString& scaleKeyword );

      template <class S>
      void UpdateImageScale( pcl_enum scaleMode, float scaleValue, const S& scaleKeyword )
      {
         UpdateImageScale( scaleMode, scaleValue, IsoString( scaleKeyword ) );
      }

      void UpdateNodes();
      void DestroyStarNodes();
      void CreateStarNodes();

      unsigned UniqueStarId()
      {
         return unsigned( nextStarId.FetchAndAdd( 1 ) );
      }

      void Sort( SortingCriterion );

   private:

      AtomicInt nextStarId = 1;

      void UpdateDetectionImage();
      void GetAstrometricSolution();

      class StarThread : public Thread
      {
      public:

         StarThread( const ImageVariant& image, float threshold, bool autoAperture, const PSFOptions& options,
                     star_list::iterator begin,
                     star_list::iterator end )
            : m_image( image )
            , m_threshold( threshold )
            , m_autoAperture( autoAperture )
            , m_options( options )
            , m_begin( begin )
            , m_end( end )
         {
         }

      protected:

         const ImageVariant& m_image;
         float               m_threshold;
         bool                m_autoAperture;
         PSFOptions          m_options;
         star_list::iterator m_begin;
         star_list::iterator m_end;
      };

      class RegenerateThread : public StarThread
      {
      public:

         RegenerateThread( const ImageVariant& image, float threshold, bool autoAperture, const PSFOptions& options,
                           star_list::iterator begin,
                           star_list::iterator end )
            : StarThread( image, threshold, autoAperture, options, begin, end )
         {
         }

         void Run() override
         {
            for ( star_list::iterator s = m_begin; s != m_end; ++s )
               s->Regenerate( m_image, m_threshold, m_autoAperture, m_options );
         }
      };

      class RecalculateThread : public StarThread
      {
      public:

         RecalculateThread( const ImageVariant& image, float threshold, bool autoAperture, const PSFOptions& options,
                            star_list::iterator begin,
                            star_list::iterator end )
            : StarThread( image, threshold, autoAperture, options, begin, end )
         {
         }

         void Run() override
         {
            for ( star_list::iterator s = m_begin; s != m_end; ++s )
               s->Recalculate( m_image, m_threshold, m_autoAperture, m_options );
         }
      };
   };

   typedef ReferenceArray<PSFCollection> psf_collection_list;

   psf_collection_list m_collections;
   star_list           m_selectedStars;
   float               m_fontSize = 8;

   //

   PSFCollection* FindPSFCollection( const View& view )
   {
      for ( PSFCollection& collection : m_collections )
         if ( collection.view == view )
            return &collection;
      return nullptr;
   }

   const PSFCollection* FindPSFCollection( const View& view ) const
   {
      return const_cast<DynamicPSFInterface*>( this )->FindPSFCollection( view );
   }

   bool HasPSFCollection( const View& view ) const
   {
      return FindPSFCollection( view ) != nullptr;
   }

   PSFCollection* AcquirePSFCollection( View& );

   //

   star_list FindStars( const View& view, const DRect& rect, ImageWindow::display_channel channel );

   void SelectStars( const star_list& stars, bool addToSelection = false );

   void SelectStar( const Star* star )
   {
      if ( star != nullptr )
      {
         star_list list;
         list << star;
         SelectStars( list );
      }
   }

   void UnselectStars()
   {
      star_list emptyList;
      SelectStars( emptyList );
   }

   void TrackStar( const Star* );

   void Regenerate();
   void Recalculate();

   void ExportCSV( const String& filePath );

   void Update()
   {
      for ( PSFCollection& collection : m_collections )
         collection.Update();
   }

   void DrawPSF( VectorGraphics& g, double penWidth, const Star&, const PSF& psf, ImageWindow& ) const;
   void DrawStar( VectorGraphics&, double penWidth, const Star&, ImageWindow&, const Rect& r0 ) const;

   Star* StarFromTreeBoxNode( TreeBox::Node& );

   void ExpandTreeBoxNodes( TreeBox::Node* );
   void CollapseTreeBoxNodes( TreeBox::Node* );

   friend struct GUIData;
   friend class PSFCollectionNode;
   friend class StarNode;
   friend class PSFNode;
   friend class AverageStarDialog;
   friend class SortStarsDialog;
   friend class StarSortingBinaryPredicate;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern DynamicPSFInterface* TheDynamicPSFInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __DynamicPSFInterface_h

// ----------------------------------------------------------------------------
// EOF DynamicPSFInterface.h - Released 2023-08-28T15:23:41Z
