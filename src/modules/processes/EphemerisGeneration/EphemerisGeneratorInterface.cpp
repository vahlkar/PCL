//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// EphemerisGeneratorInterface.cpp - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
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

#include "Elements.h"
#include "Ephemerides.h"
#include "EphemerisGeneratorInterface.h"
#include "EphemerisGeneratorParameters.h"
#include "EphemerisGeneratorProcess.h"

#include <pcl/CodeEditor.h>
#include <pcl/Dialog.h>
#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/Position.h>

#define DEFAULT_CORE_XEPH_FILE_TAG  "<core-default>"

namespace pcl
{

// ----------------------------------------------------------------------------

EphemerisGeneratorInterface* TheEphemerisGeneratorInterface = nullptr;

// ----------------------------------------------------------------------------

EphemerisGeneratorInterface::EphemerisGeneratorInterface()
   : m_instance( TheEphemerisGeneratorProcess )
{
   TheEphemerisGeneratorInterface = this;
}

// ----------------------------------------------------------------------------

EphemerisGeneratorInterface::~EphemerisGeneratorInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString EphemerisGeneratorInterface::Id() const
{
   return "EphemerisGenerator";
}

// ----------------------------------------------------------------------------

MetaProcess* EphemerisGeneratorInterface::Process() const
{
   return TheEphemerisGeneratorProcess;
}

// ----------------------------------------------------------------------------

String EphemerisGeneratorInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/EphemerisGenerator.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures EphemerisGeneratorInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

// ----------------------------------------------------------------------------

void EphemerisGeneratorInterface::ApplyInstance() const
{
   m_instance.LaunchGlobal();
}

// ----------------------------------------------------------------------------

void EphemerisGeneratorInterface::ResetInstance()
{
   EphemerisGeneratorInstance defaultInstance( TheEphemerisGeneratorProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool EphemerisGeneratorInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "EphemerisGenerator" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheEphemerisGeneratorProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* EphemerisGeneratorInterface::NewProcess() const
{
   return new EphemerisGeneratorInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool EphemerisGeneratorInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const EphemerisGeneratorInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a EphemerisGenerator instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool EphemerisGeneratorInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool EphemerisGeneratorInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Array<TextDatabase::ObjectData> EphemerisGeneratorInterface::SearchDatabase( bool& gotElements, int maxCount ) const
{
   if ( m_instance.p_databaseFilePath.IsEmpty() )
      throw Error( "EphemerisGenerator: No database file has been specified" );

   int formatIndex = EphemerisGeneratorProcess::FindDatabaseFormatByName( m_instance.p_databaseFormatName );
   if ( formatIndex < 0 )
      throw Error( "EphemerisGenerator: Database format not available: '" + m_instance.p_databaseFormatName + '\'' );

   TextDatabase::FormatDescription format = EphemerisGeneratorProcess::DatabaseFormats()[formatIndex];
   TextDatabase database( format );
   Array<TextDatabase::ObjectData> objects = database.Search( m_instance, maxCount );
   if ( objects.IsEmpty() )
      throw Error( "EphemerisGenerator: No objects found" );

   gotElements = format.contents == TextDatabase::Contents_OrbitalElements;
   return objects;
}

// ----------------------------------------------------------------------------

String EphemerisGeneratorInterface::TimeParameterValueToString( double jd )
{
   return TimePoint( jd ).ToString( 3/*timeItems*/, 3/*precision*/, 0/*tz*/, false/*timeZone*/ );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void EphemerisGeneratorInterface::UpdateControls()
{
   GUI->WorkingMode_ComboBox.SetCurrentItem( m_instance.p_workingMode );

   GUI->StateVectors_GroupBox.SetChecked( m_instance.p_workingMode == EGWorkingMode::StateVectors );

   GUI->X_NumericEdit.SetValue( m_instance.p_position[0] );
   GUI->Y_NumericEdit.SetValue( m_instance.p_position[1] );
   GUI->Z_NumericEdit.SetValue( m_instance.p_position[2] );

   GUI->XDot_NumericEdit.SetValue( m_instance.p_velocity[0] );
   GUI->YDot_NumericEdit.SetValue( m_instance.p_velocity[1] );
   GUI->ZDot_NumericEdit.SetValue( m_instance.p_velocity[2] );

   GUI->OrbitalElements_GroupBox.SetChecked( m_instance.p_workingMode == EGWorkingMode::OrbitalElements );

   GUI->A_NumericEdit.SetValue( m_instance.el_a );
   GUI->Q_NumericEdit.SetValue( m_instance.el_q );
   GUI->E_NumericEdit.SetValue( m_instance.el_e );
   GUI->I_NumericEdit.SetValue( m_instance.el_i );
   GUI->M_NumericEdit.SetValue( m_instance.el_M );
   GUI->T_NumericEdit.SetValue( m_instance.el_T );
   GUI->O_NumericEdit.SetValue( m_instance.el_O );
   GUI->W_NumericEdit.SetValue( m_instance.el_w );

   GUI->Epoch_Edit.SetText( TimeParameterValueToString( m_instance.p_epochJD ) );

   GUI->ObjectId_Edit.SetText( m_instance.p_objectId );
   GUI->ObjectName_Edit.SetText( m_instance.p_objectName );

   GUI->H_NumericEdit.SetValue( m_instance.p_H );
   GUI->G_NumericEdit.SetValue( m_instance.p_G );
   GUI->B_V_NumericEdit.SetValue( m_instance.p_B_V );
   GUI->B_V_CheckBox.SetChecked( m_instance.p_B_V_defined );
   GUI->D_NumericEdit.SetValue( m_instance.p_D );
   GUI->D_CheckBox.SetChecked( m_instance.p_D_defined );

   GUI->B_V_NumericEdit.Enable( m_instance.p_B_V_defined );
   GUI->D_NumericEdit.Enable( m_instance.p_D_defined );

   GUI->ObjectParameters_Control.Enable( m_instance.p_workingMode != EGWorkingMode::DatabaseObjects );

   GUI->Database_GroupBox.SetChecked( m_instance.p_workingMode == EGWorkingMode::DatabaseObjects );

   GUI->DatabasePath_Edit.SetText( m_instance.p_databaseFilePath );

   GUI->DatabaseFormat_ComboBox.SetCurrentItem(
            EphemerisGeneratorProcess::FindDatabaseFormatByName( m_instance.p_databaseFormatName ) );

   GUI->DatabaseObjects_TextBox.SetPlainText( m_instance.p_objects );

   GUI->UseRegularExpressions_CheckBox.SetChecked( m_instance.p_useRegularExpressions );
   GUI->ExcludeCoreAsteroids_CheckBox.SetChecked( m_instance.p_excludeCoreAsteroids );

   GUI->FundamentalEphemerides_Edit.SetText( m_instance.p_fundamentalFilePath.IsEmpty() ?
                                    DEFAULT_CORE_XEPH_FILE_TAG : m_instance.p_fundamentalFilePath );

   GUI->AsteroidPerturbers_CheckBox.SetChecked( m_instance.p_useAsteroidPerturbers );

   GUI->AsteroidEphemerides_Edit.SetText( m_instance.p_asteroidsFilePath.IsEmpty() ?
                                    DEFAULT_CORE_XEPH_FILE_TAG : m_instance.p_asteroidsFilePath );
   GUI->AsteroidEphemerides_Edit.Enable( m_instance.p_useAsteroidPerturbers );
   GUI->AsteroidEphemerides_ToolButton.Enable( m_instance.p_useAsteroidPerturbers );

   GUI->KBOPerturbers_CheckBox.SetChecked( m_instance.p_useKBOPerturbers );

   GUI->KBOEphemerides_Edit.SetText( m_instance.p_KBOsFilePath.IsEmpty() ?
                                    DEFAULT_CORE_XEPH_FILE_TAG : m_instance.p_KBOsFilePath );
   GUI->KBOEphemerides_Edit.Enable( m_instance.p_useKBOPerturbers );
   GUI->KBOEphemerides_ToolButton.Enable( m_instance.p_useKBOPerturbers );

//    GUI->SeparateEarthMoon_CheckBox.SetChecked( m_instance.p_separateEarthMoonPerturbers );

   GUI->StartTime_Edit.SetText( TimeParameterValueToString( m_instance.p_startTimeJD ) );
   GUI->EndTime_Edit.SetText( TimeParameterValueToString( m_instance.p_endTimeJD ) );

   GUI->OutputXEPHFile_CheckBox.SetChecked( m_instance.p_outputXEPHFile );
   GUI->OutputXEPHFilePath_Edit.SetText( m_instance.p_outputXEPHFilePath );

   GUI->OutputXEPHFilePath_Label.Enable( m_instance.p_outputXEPHFile );
   GUI->OutputXEPHFilePath_Edit.Enable( m_instance.p_outputXEPHFile );
   GUI->OutputXEPHFilePath_ToolButton.Enable( m_instance.p_outputXEPHFile );

   GUI->OverwriteExistingFiles_CheckBox.Enable( m_instance.p_outputXEPHFile );
   GUI->OverwriteExistingFiles_CheckBox.SetChecked( m_instance.p_overwriteExistingFiles );
}

// ----------------------------------------------------------------------------

void EphemerisGeneratorInterface::UpdateElementDependencies( int idx )
{
   switch ( idx )
   {
   case a_idx:
      m_instance.el_q = PerihelionDistanceFromSemimajorAxis( m_instance.p_elements );
      break;
   case q_idx:
      m_instance.el_a = SemimajorAxisFromPerihelionDistance( m_instance.p_elements );
      break;
   case e_idx:
      if ( NearParabolic( m_instance.p_elements ) )
         m_instance.el_a = SemimajorAxisFromPerihelionDistance( m_instance.p_elements );
      else
         m_instance.el_q = PerihelionDistanceFromSemimajorAxis( m_instance.p_elements );
      break;
   case M_idx:
      m_instance.el_T = TimeOfPerihelionPassageFromMeanAnomaly( m_instance.p_elements, m_instance.p_epochJD );
      break;
   case T_idx:
      m_instance.el_M = MeanAnomalyFromTimeOfPerihelionPassage( m_instance.p_elements, m_instance.p_epochJD );
      break;
   default:
      break;
   }

   switch ( idx )
   {
   case a_idx:
   case q_idx:
   case e_idx:
      if ( NearParabolic( m_instance.p_elements ) )
         m_instance.el_M = MeanAnomalyFromTimeOfPerihelionPassage( m_instance.p_elements, m_instance.p_epochJD );
      else
         m_instance.el_T = TimeOfPerihelionPassageFromMeanAnomaly( m_instance.p_elements, m_instance.p_epochJD );
      break;
   default:
      break;
   }

   UpdateControls();
}

// ----------------------------------------------------------------------------

void EphemerisGeneratorInterface::e_TimeEditCompleted( Edit& sender )
{
   double jd;
   if ( sender == GUI->Epoch_Edit )
      jd = m_instance.p_epochJD;
   else if ( sender == GUI->StartTime_Edit )
      jd = m_instance.p_startTimeJD;
   else if ( sender == GUI->EndTime_Edit )
      jd = m_instance.p_endTimeJD;
   else
      return; // ?!

   String text = sender.Text().Trimmed();
   if ( !text.IsEmpty() )
   {
      int year, month = 1, day = 1;
      double dayf = 0, tz = 0;

      if ( text.TryParseISO8601DateTime( year, month, day, dayf, tz ) )
      {
         jd = CalendarTimeToJD( year, month, day, dayf - tz/24 );
      }
      else
      {
         try
         {
            StringList tokens;
            text.Break( tokens, ' ', true/*trim*/ );
            tokens.Remove( String() );
            switch ( tokens.Length() )
            {
            case 3:
               year = tokens[0].ToInt();
               month = tokens[1].ToInt();
               dayf = tokens[2].ToDouble();
               dayf -= (day = TruncInt( dayf ));
               jd = CalendarTimeToJD( year, month, day, dayf );
               break;
            case 1:
               jd = tokens[0].ToDouble();
               break;
            case 2:
               throw Error( "Too few date items - year, month and day (with optional fraction) are required." );
            default:
               throw Error( "Too many date items - year, month and day (with optional fraction) suffice." );
            }
         }
         ERROR_HANDLER
      }

      if ( sender == GUI->Epoch_Edit )
         m_instance.p_epochJD = jd;
      else if ( sender == GUI->StartTime_Edit )
         m_instance.p_startTimeJD = jd;
      else if ( sender == GUI->EndTime_Edit )
         m_instance.p_endTimeJD = jd;
   }

   sender.SetText( TimeParameterValueToString( jd ) );
}

// ----------------------------------------------------------------------------

void EphemerisGeneratorInterface::e_IdEditCompleted( Edit& sender )
{
   /*
    * Only printable ASCII characters are allowed in XEPH object identifiers
    * and names. Replace outsiders with underscores.
    */
   String text = sender.Text().Trimmed();
   for ( auto& c : text )
      if ( c < 32 || c > 126 )
         c = '_';

   if ( sender == GUI->ObjectId_Edit )
      m_instance.p_objectId = text;
   else if ( sender == GUI->ObjectName_Edit )
      m_instance.p_objectName = text;

   sender.SetText( text );
}

// ----------------------------------------------------------------------------

void EphemerisGeneratorInterface::e_FilePathEditCompleted( Edit& sender )
{
   String text = sender.Text().Trimmed();
   if ( sender == GUI->FundamentalEphemerides_Edit || sender == GUI->AsteroidEphemerides_Edit || sender == GUI->KBOEphemerides_Edit )
      if ( text == DEFAULT_CORE_XEPH_FILE_TAG )
         text.Clear();

   if ( sender == GUI->DatabasePath_Edit )
      m_instance.p_databaseFilePath = text;
   else if ( sender == GUI->FundamentalEphemerides_Edit )
      m_instance.p_fundamentalFilePath = text;
   else if ( sender == GUI->AsteroidEphemerides_Edit )
      m_instance.p_asteroidsFilePath = text;
   else if ( sender == GUI->KBOEphemerides_Edit )
      m_instance.p_KBOsFilePath = text;
   else if ( sender == GUI->OutputXEPHFilePath_Edit )
      m_instance.p_outputXEPHFilePath = text;

   if ( sender == GUI->FundamentalEphemerides_Edit || sender == GUI->AsteroidEphemerides_Edit || sender == GUI->KBOEphemerides_Edit )
      if ( text.IsEmpty() )
         text = DEFAULT_CORE_XEPH_FILE_TAG;

   sender.SetText( text );
}

// ----------------------------------------------------------------------------

void EphemerisGeneratorInterface::e_FilePathEditGetFocus( Control& sender )
{
   Edit& edit = static_cast<Edit&>( sender );
   if ( edit == GUI->FundamentalEphemerides_Edit || edit == GUI->AsteroidEphemerides_Edit || edit == GUI->KBOEphemerides_Edit )
      if ( edit.Text().Trimmed() == DEFAULT_CORE_XEPH_FILE_TAG )
         edit.Clear();
}

// ----------------------------------------------------------------------------

void EphemerisGeneratorInterface::e_FilePathEditLoseFocus( Control& sender )
{
   Edit& edit = static_cast<Edit&>( sender );
   if ( edit == GUI->FundamentalEphemerides_Edit || edit == GUI->AsteroidEphemerides_Edit || edit == GUI->KBOEphemerides_Edit )
      if ( edit.Text().Trimmed().IsEmpty() )
         edit.SetText( DEFAULT_CORE_XEPH_FILE_TAG );
}

// ----------------------------------------------------------------------------

void EphemerisGeneratorInterface::e_EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->X_NumericEdit )
      m_instance.p_position[0] = value;
   else if ( sender == GUI->Y_NumericEdit )
      m_instance.p_position[1] = value;
   else if ( sender == GUI->Z_NumericEdit )
      m_instance.p_position[2] = value;
   else if ( sender == GUI->XDot_NumericEdit )
      m_instance.p_velocity[0] = value;
   else if ( sender == GUI->YDot_NumericEdit )
      m_instance.p_velocity[1] = value;
   else if ( sender == GUI->ZDot_NumericEdit )
      m_instance.p_velocity[2] = value;

   else if ( sender == GUI->A_NumericEdit )
   {
      m_instance.el_a = value;
      UpdateElementDependencies( a_idx );
   }
   else if ( sender == GUI->Q_NumericEdit )
   {
      m_instance.el_q = value;
      UpdateElementDependencies( q_idx );
   }
   else if ( sender == GUI->M_NumericEdit )
   {
      m_instance.el_M = value;
      UpdateElementDependencies( M_idx );
   }
   else if ( sender == GUI->T_NumericEdit )
   {
      m_instance.el_T = value;
      UpdateElementDependencies( T_idx );
   }
   else if ( sender == GUI->E_NumericEdit )
   {
      m_instance.el_e = value;
      UpdateElementDependencies( e_idx );
   }
   else if ( sender == GUI->O_NumericEdit )
      m_instance.el_O = value;
   else if ( sender == GUI->I_NumericEdit )
      m_instance.el_i = value;
   else if ( sender == GUI->W_NumericEdit )
      m_instance.el_w = value;

   else if ( sender == GUI->H_NumericEdit )
      m_instance.p_H = value;
   else if ( sender == GUI->G_NumericEdit )
      m_instance.p_G = value;
   else if ( sender == GUI->B_V_NumericEdit )
      m_instance.p_B_V = value;
   else if ( sender == GUI->D_NumericEdit )
      m_instance.p_D = value;
}

// ----------------------------------------------------------------------------

class ObjectDataDialog : public Dialog
{
public:

   ObjectDataDialog( const String& text )
   {
      Data_Editor.SetScaledMinSize( 800, 600 );
      Data_Editor.SetReadOnly();

      Dismiss_PushButton.SetText( "OK" );
      Dismiss_PushButton.SetDefault();
      Dismiss_PushButton.OnClick( (pcl::Button::click_event_handler)&ObjectDataDialog::e_Click, *this );

      Buttons_Sizer.AddStretch();
      Buttons_Sizer.Add( Dismiss_PushButton );

      Global_Sizer.SetMargin( 8 );
      Global_Sizer.Add( Data_Editor );
      Global_Sizer.AddSpacing( 8 );
      Global_Sizer.Add( Buttons_Sizer );

      SetSizer( Global_Sizer );

      EnsureLayoutUpdated();
      AdjustToContents();

      SetWindowTitle( "EphemerisGenerator | Database Search Result" );

      Data_Editor.SetText( text );
   }

private:

   VerticalSizer  Global_Sizer;
      CodeEditor        Data_Editor;
      HorizontalSizer   Buttons_Sizer;
         PushButton        Dismiss_PushButton;

   void e_Click( Button& sender, bool checked )
   {
      if ( sender == Dismiss_PushButton )
         Ok();
   }
};

void EphemerisGeneratorInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->ToElements_ToolButton )
   {
      Vector r = m_instance.p_position;
      Vector v = m_instance.p_velocity;

      Ephemerides eph( m_instance, true/*fundamentalOnly*/ );
      EphemerisFile::Handle sun( eph.Fundamental(), "Sn", "SSB" );
      Vector r0, v0;
      sun.ComputeState( r0, v0, m_instance.p_epochJD );
      r -= r0;
      v -= v0;

      pcl::Position P( TimePoint::J2000(), "TDB" );
      r = Position::EquatorialToEcliptic( r, P.EpsA() );
      v = Position::EquatorialToEcliptic( v, P.EpsA() );

      Vector el;
      OrbitalElementsFromStateVectors( el, r, v, m_instance.p_epochJD, eph.GMS() );
      m_instance.p_elements = el;
      m_instance.p_workingMode = EGWorkingMode::OrbitalElements;
      UpdateControls();
   }
   else if ( sender == GUI->ToVectors_ToolButton )
   {
      Ephemerides eph( m_instance, true/*fundamentalOnly*/ );
      Vector r, v;
      StateVectorsFromOrbitalElements( r, v, m_instance.p_elements, m_instance.p_epochJD, eph.GMS() );

      pcl::Position P( TimePoint::J2000(), "TDB" );
      r = Position::EclipticToEquatorial( r, P.EpsA() );
      v = Position::EclipticToEquatorial( v, P.EpsA() );

      EphemerisFile::Handle sun( eph.Fundamental(), "Sn", "SSB" );
      Vector r0, v0;
      sun.ComputeState( r0, v0, m_instance.p_epochJD );
      r += r0;
      v += v0;

      m_instance.p_position = r;
      m_instance.p_velocity = v;
      m_instance.p_workingMode = EGWorkingMode::StateVectors;
      UpdateControls();
   }
   else if ( sender == GUI->B_V_CheckBox )
   {
      m_instance.p_B_V_defined = checked;
      UpdateControls();
   }
   else if ( sender == GUI->D_CheckBox )
   {
      m_instance.p_D_defined = checked;
      UpdateControls();
   }
   else if ( sender == GUI->UseRegularExpressions_CheckBox )
   {
      m_instance.p_useRegularExpressions = checked;
   }
   else if ( sender == GUI->ExcludeCoreAsteroids_CheckBox )
   {
      m_instance.p_excludeCoreAsteroids = checked;
   }
   else if ( sender == GUI->DatabasePath_ToolButton )
   {
      OpenFileDialog d;
      d.DisableMultipleSelections();
      d.SetFilter( FileFilter( "Any Files", StringList() << "*" ) );
      d.SetCaption( "Select Object Database File" );
      if ( d.Execute() )
      {
         m_instance.p_databaseFilePath = d.FileName();
         UpdateControls();
      }
   }
   else if ( sender == GUI->GetFirst_Button )
   {
      bool gotElements;
      Array<TextDatabase::ObjectData> objects = SearchDatabase( gotElements, 1/*maxCount*/ );

      if ( gotElements )
      {
         m_instance.p_workingMode = EGWorkingMode::OrbitalElements;
         m_instance.p_elements = objects[0].state;
      }
      else // Contents_StateVectors
      {
         m_instance.p_workingMode = EGWorkingMode::StateVectors;
         m_instance.p_position = Vector( objects[0].state[0], objects[0].state[1], objects[0].state[2] );
         m_instance.p_velocity = Vector( objects[0].state[3], objects[0].state[4], objects[0].state[5] );
      }

      m_instance.p_epochJD = objects[0].epochJD;
      m_instance.p_objectId = objects[0].id;
      m_instance.p_objectName = objects[0].name;
      m_instance.p_H = objects[0].H.OrElse( 14.0 );
      m_instance.p_G = objects[0].G.OrElse( 0.15 );
      m_instance.p_B_V_defined = objects[0].B_V.IsDefined();
      m_instance.p_B_V = objects[0].B_V.OrElse( 0.0 );
      m_instance.p_D_defined = objects[0].D.IsDefined();
      m_instance.p_D = objects[0].D.OrElse( 0.0 );

      UpdateControls();
   }
   else if ( sender == GUI->CheckObjects_Button )
   {
      bool gotElements;
      Array<TextDatabase::ObjectData> objects = SearchDatabase( gotElements, 100000/*maxCount*/ );

      size_type idLength = 10, nameLength = 24;
      for ( const TextDatabase::ObjectData& o : objects )
      {
         if ( o.id.Length() > idLength )
            idLength = o.id.Length();
         if ( o.name.Length() > nameLength )
            nameLength = o.name.Length();
      }

      String idHdr = String( "Id" ).CenterJustified( idLength );
      String nameHdr = String( "Name" ).CenterJustified( nameLength );
      String idHdr1( '-', idLength );
      String nameHdr1( '-', nameLength );
      String idHdr2( ' ', idLength );
      String nameHdr2( ' ', nameLength );
      String idHdr3( '=', idLength );
      String nameHdr3( '=', nameLength );

      String text;
      if ( gotElements )
         text <<
"  #    " << idHdr << ' ' << nameHdr <<   "     Epoch       H    G         a              q             e            i             M            T            L.Node       A.Per\n"
"------ " << idHdr1 << ' ' << nameHdr1 << " ------------- ----- ---- -------------- -------------- ------------ ------------ ------------ ---------------- ----------- -----------\n"
"       " << idHdr2 << ' ' << nameHdr2 << "     JD TDB      mag         au             au                           deg          deg           JD TDB         deg         deg\n"
"====== " << idHdr3 << ' ' << nameHdr3 << " ============= ===== ==== ============== ============== ============ ============ ============ ================ =========== ===========\n";
      else
         text <<
"  #    " << idHdr << ' ' << nameHdr <<   "     Epoch       H    G         x              y              z             vx            vy            vz\n"
"------ " << idHdr1 << ' ' << nameHdr1 << " ------------- ----- ---- -------------- -------------- -------------- ------------- ------------- -------------\n"
"       " << idHdr2 << ' ' << nameHdr2 << "     JD TDB      mag         au             au             au            au/day        au/day        au/day\n"
"====== " << idHdr3 << ' ' << nameHdr3 << " ============= ===== ==== ============== ============== ============== ============= ============= =============\n";

      int n = 1;
      for ( TextDatabase::ObjectData& o : objects )
      {
         text << String( n++ ).RightJustified( 6 ) << ' '
              << o.id.RightJustified( idLength ) << ' '
              << o.name.LeftJustified( nameLength ) << ' '
              << String().Format( "%13.5f", o.epochJD ) << ' '
              << (o.H.IsDefined() ? String().Format( "%5.2f", o.H() ) : String( ' ', 5 )) << ' '
              << (o.G.IsDefined() ? String().Format( "%4.2f", o.G() ) : String( ' ', 4 )) << ' ';

         if ( gotElements )
            text << String().Format( "%14.10f %14.10f %12.10f %12.7f %12.7f %16.7f %11.7f %11.7f\n"
                                   , o.state[a_idx], o.state[q_idx], o.state[e_idx], o.state[i_idx]
                                   , o.state[M_idx], o.state[T_idx], o.state[O_idx], o.state[w_idx] );
         else // vectors
            text << String().Format( "%+14.10f %+14.10f %+14.10f %+13.10f %+13.10f %+13.10f\n"
                                   , o.state[0], o.state[1], o.state[2]
                                   , o.state[3], o.state[4], o.state[5] );
      }

      ObjectDataDialog( text ).Execute();
   }
   else if ( sender == GUI->FundamentalEphemerides_ToolButton )
   {
      OpenFileDialog d;
      d.DisableMultipleSelections();
      d.SetFilter( FileFilter( "XEPH Ephemeris Files", StringList() << ".xeph" ) );
      d.SetCaption( "Select Fundamental Ephemerides File" );
      d.SetInitialPath( m_instance.p_fundamentalFilePath );
      if ( d.Execute() )
      {
         m_instance.p_fundamentalFilePath = d.FileName();
         UpdateControls();
      }
   }
   else if ( sender == GUI->AsteroidPerturbers_CheckBox )
   {
      m_instance.p_useAsteroidPerturbers = checked;
      UpdateControls();
   }
   else if ( sender == GUI->AsteroidEphemerides_ToolButton )
   {
      OpenFileDialog d;
      d.DisableMultipleSelections();
      d.SetFilter( FileFilter( "XEPH Ephemeris Files", StringList() << ".xeph" ) );
      d.SetCaption( "Select Asteroid Ephemerides File" );
      d.SetInitialPath( m_instance.p_asteroidsFilePath );
      if ( d.Execute() )
      {
         m_instance.p_asteroidsFilePath = d.FileName();
         UpdateControls();
      }
   }
   else if ( sender == GUI->KBOPerturbers_CheckBox )
   {
      m_instance.p_useKBOPerturbers = checked;
      UpdateControls();
   }
   else if ( sender == GUI->KBOEphemerides_ToolButton )
   {
      OpenFileDialog d;
      d.DisableMultipleSelections();
      d.SetFilter( FileFilter( "XEPH Ephemeris Files", StringList() << ".xeph" ) );
      d.SetCaption( "Select KBO Ephemerides File" );
      d.SetInitialPath( m_instance.p_KBOsFilePath );
      if ( d.Execute() )
      {
         m_instance.p_KBOsFilePath = d.FileName();
         UpdateControls();
      }
   }
   else if ( sender == GUI->OutputXEPHFile_CheckBox )
   {
      m_instance.p_outputXEPHFile = checked;
      UpdateControls();
   }
   else if ( sender == GUI->OutputXEPHFilePath_ToolButton )
   {
      SaveFileDialog d;
      d.SetFilter( FileFilter( "XEPH Ephemeris Files", StringList() << ".xeph" ) );
      d.SetCaption( "Select Output Ephemerides File" );
      d.SetInitialPath( m_instance.p_outputXEPHFilePath );
      d.EnableOverwritePrompt();
      if ( d.Execute() )
      {
         m_instance.p_outputXEPHFilePath = d.FileName();
         UpdateControls();
      }
   }
   else if ( sender == GUI->OverwriteExistingFiles_CheckBox )
   {
      m_instance.p_overwriteExistingFiles = checked;
   }
}

// ----------------------------------------------------------------------------

void EphemerisGeneratorInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->WorkingMode_ComboBox )
   {
      m_instance.p_workingMode = itemIndex;
      UpdateControls();
   }
   else if ( sender == GUI->DatabaseFormat_ComboBox )
      m_instance.p_databaseFormatName = EphemerisGeneratorProcess::DatabaseFormats()[itemIndex].name;
}

// ----------------------------------------------------------------------------

void EphemerisGeneratorInterface::e_TextUpdated( TextBox& sender, const String& text )
{
   if ( sender == GUI->DatabaseObjects_TextBox )
      m_instance.p_objects = text.Trimmed();
}

// ----------------------------------------------------------------------------

void EphemerisGeneratorInterface::e_GroupBoxCheck( GroupBox& sender, bool checked )
{
   if ( sender == GUI->StateVectors_GroupBox )
      m_instance.p_workingMode = EGWorkingMode::StateVectors;
   else if ( sender == GUI->OrbitalElements_GroupBox )
      m_instance.p_workingMode = EGWorkingMode::OrbitalElements;
   else if ( sender == GUI->Database_GroupBox )
      m_instance.p_workingMode = EGWorkingMode::DatabaseObjects;

   UpdateControls();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

EphemerisGeneratorInterface::GUIData::GUIData( EphemerisGeneratorInterface& w )
{
   pcl::Font font = w.Font();
   int labelWidth1 = font.Width( String( "Fundamental ephemerides:" ) + 'X' );
   int labelWidth2 = font.Width( String( "B-V:" ) + 'l' );
   int ui4 = w.LogicalPixelsToPhysical( 4 );
   int ui9 = w.LogicalPixelsToPhysical( 9 );

   //

   String crdStyleSheet = w.ScaledStyleSheet(
      "QWidget {"
         "font-family: Hack, DejaVu Sans Mono;"
         "font-size: 12px;"
         "padding: 0;"
      "}" );

   String lblStyleSheet_i = w.ScaledStyleSheet(
      "QLabel {"
         "font-family: DejaVu Serif;"
         "font-style: italic;"
         "font-size: 16px;"
      "}" );

   String lblStyleSheet_n = w.ScaledStyleSheet(
      "QLabel {"
         "font-family: DejaVu Serif;"
         "font-size: 16px;"
      "}" );

   //

   InitialConditions_SectionBar.SetTitle( "Initial Conditions" );
   InitialConditions_SectionBar.SetSection( InitialConditions_Control );
   InitialConditions_SectionBar.SetToolTip( "<p>Initial values for numerical integration.</p>" );

   //

   const char* workingModeToolTip = "<p></p>";

   WorkingMode_Label.SetText( "Working mode:" );
   WorkingMode_Label.SetFixedWidth( labelWidth1 );
   WorkingMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   WorkingMode_Label.SetToolTip( workingModeToolTip );

   WorkingMode_ComboBox.AddItem( "State vectors / single object" );
   WorkingMode_ComboBox.AddItem( "Orbital elements / single object" );
   WorkingMode_ComboBox.AddItem( "Database / multiple objects" );
   WorkingMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&EphemerisGeneratorInterface::e_ItemSelected, w );

   WorkingMode_Sizer.SetSpacing( 4 );
   WorkingMode_Sizer.Add( WorkingMode_Label );
   WorkingMode_Sizer.Add( WorkingMode_ComboBox );
   WorkingMode_Sizer.AddStretch();

   //

   StateVectors_GroupBox.SetTitle( "State Vectors" );
   StateVectors_GroupBox.EnableTitleCheckBox();
   StateVectors_GroupBox.OnCheck( (GroupBox::check_event_handler)&EphemerisGeneratorInterface::e_GroupBoxCheck, w );

   //

   X_NumericEdit.label.SetStyleSheet( lblStyleSheet_i );
   X_NumericEdit.label.SetText( u"x\u2009" );
   X_NumericEdit.SetReal();
   X_NumericEdit.EnableFixedPrecision();
   X_NumericEdit.EnableFixedSign();
   X_NumericEdit.SetRange( TheEGPosXParameter->MinimumValue(), TheEGPosXParameter->MaximumValue() );
   X_NumericEdit.SetPrecision( TheEGPosXParameter->Precision() );
   X_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   X_NumericEdit.SetToolTip( "<p>Position x-coordinate at the epoch (au).</p>" );
   X_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   XDot_NumericEdit.label.SetStyleSheet( lblStyleSheet_i );
   XDot_NumericEdit.label.SetText( u"\u1E8B\u2009" );
   XDot_NumericEdit.SetReal();
   XDot_NumericEdit.EnableFixedPrecision();
   XDot_NumericEdit.EnableFixedSign();
   XDot_NumericEdit.SetRange( TheEGVelXParameter->MinimumValue(), TheEGVelXParameter->MaximumValue() );
   XDot_NumericEdit.SetPrecision( TheEGVelXParameter->Precision() );
   XDot_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   XDot_NumericEdit.SetToolTip( "<p>Velocity x-coordinate at the epoch (au/day).</p>" );
   XDot_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   X_Sizer.Add( X_NumericEdit );
   X_Sizer.AddSpacing( 12 );
   X_Sizer.Add( XDot_NumericEdit );
   X_Sizer.AddStretch();

   //

   Y_NumericEdit.label.SetStyleSheet( lblStyleSheet_i );
   Y_NumericEdit.label.SetText( u"y\u2009" );
   Y_NumericEdit.SetReal();
   Y_NumericEdit.EnableFixedPrecision();
   Y_NumericEdit.EnableFixedSign();
   Y_NumericEdit.SetRange( TheEGPosYParameter->MinimumValue(), TheEGPosYParameter->MaximumValue() );
   Y_NumericEdit.SetPrecision( TheEGPosYParameter->Precision() );
   Y_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   Y_NumericEdit.SetToolTip( "<p>Position y-coordinate at the epoch (au).</p>" );
   Y_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   YDot_NumericEdit.label.SetStyleSheet( lblStyleSheet_i );
   YDot_NumericEdit.label.SetText( u"\u1E8F\u2009" );
   YDot_NumericEdit.SetReal();
   YDot_NumericEdit.EnableFixedPrecision();
   YDot_NumericEdit.EnableFixedSign();
   YDot_NumericEdit.SetRange( TheEGVelYParameter->MinimumValue(), TheEGVelYParameter->MaximumValue() );
   YDot_NumericEdit.SetPrecision( TheEGVelYParameter->Precision() );
   YDot_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   YDot_NumericEdit.SetToolTip( "<p>Velocity y-coordinate at the epoch (au/day).</p>" );
   YDot_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   Y_Sizer.Add( Y_NumericEdit );
   Y_Sizer.AddSpacing( 12 );
   Y_Sizer.Add( YDot_NumericEdit );
   Y_Sizer.AddStretch();

   //

   Z_NumericEdit.label.SetStyleSheet( lblStyleSheet_i );
   Z_NumericEdit.label.SetText( u"z\u2009" );
   Z_NumericEdit.SetReal();
   Z_NumericEdit.EnableFixedPrecision();
   Z_NumericEdit.EnableFixedSign();
   Z_NumericEdit.SetRange( TheEGPosZParameter->MinimumValue(), TheEGPosZParameter->MaximumValue() );
   Z_NumericEdit.SetPrecision( TheEGPosZParameter->Precision() );
   Z_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   Z_NumericEdit.SetToolTip( "<p>Position z-coordinate at the epoch (au).</p>" );
   Z_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   ZDot_NumericEdit.label.SetStyleSheet( lblStyleSheet_i );
   ZDot_NumericEdit.label.SetText( u"\u017C\u2009" );
   ZDot_NumericEdit.SetReal();
   ZDot_NumericEdit.EnableFixedPrecision();
   ZDot_NumericEdit.EnableFixedSign();
   ZDot_NumericEdit.SetRange( TheEGVelZParameter->MinimumValue(), TheEGVelZParameter->MaximumValue() );
   ZDot_NumericEdit.SetPrecision( TheEGVelZParameter->Precision() );
   ZDot_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   ZDot_NumericEdit.SetToolTip( "<p>Velocity z-coordinate at the epoch (au/day).</p>" );
   ZDot_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   ToElements_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/arrows/arrow-down.png" ) ) );
   ToElements_ToolButton.SetScaledFixedSize( 20, 20 );
   ToElements_ToolButton.SetToolTip( "<p>Compute classical orbital elements from state vectors.</p>" );
   ToElements_ToolButton.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   Z_Sizer.Add( Z_NumericEdit );
   Z_Sizer.AddSpacing( 12 );
   Z_Sizer.Add( ZDot_NumericEdit );
   Z_Sizer.AddSpacing( 12 );
   Z_Sizer.Add( ToElements_ToolButton );
   Z_Sizer.AddStretch();

   //

   StateVectors_Sizer.SetMargin( 8 );
   StateVectors_Sizer.SetSpacing( 8 );
   StateVectors_Sizer.Add( X_Sizer );
   StateVectors_Sizer.Add( Y_Sizer );
   StateVectors_Sizer.Add( Z_Sizer );

   StateVectors_GroupBox.SetSizer( StateVectors_Sizer );

   //

   OrbitalElements_GroupBox.SetTitle( "Orbital Elements" );
   OrbitalElements_GroupBox.EnableTitleCheckBox();
   OrbitalElements_GroupBox.OnCheck( (GroupBox::check_event_handler)&EphemerisGeneratorInterface::e_GroupBoxCheck, w );

   //

   A_NumericEdit.label.SetStyleSheet( lblStyleSheet_i );
   A_NumericEdit.label.SetText( u"a\u2009" );
   A_NumericEdit.SetReal();
   A_NumericEdit.EnableFixedPrecision();
   A_NumericEdit.SetRange( TheEGElemAParameter->MinimumValue(), TheEGElemAParameter->MaximumValue() );
   A_NumericEdit.SetPrecision( TheEGElemAParameter->Precision() );
   A_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   A_NumericEdit.SetToolTip( "<p>Semimajor axis at the epoch (au).</p>"
                        "<p>This parameter must be &gt; 0 for elliptical and (near-)parabolic orbits, "
                        "&lt; 0 for hyperbolic orbits.</p>" );
   A_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   M_NumericEdit.label.SetStyleSheet( lblStyleSheet_i );
   M_NumericEdit.label.SetText( u"M\u2009" );
   M_NumericEdit.SetReal();
   M_NumericEdit.EnableFixedPrecision();
   M_NumericEdit.SetRange( TheEGElemMParameter->MinimumValue(), TheEGElemMParameter->MaximumValue() );
   M_NumericEdit.SetPrecision( TheEGElemMParameter->Precision() );
   M_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   M_NumericEdit.SetToolTip( "<p>Mean anomaly at the epoch (degrees).</p>" );
   M_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   AM_Sizer.Add( A_NumericEdit );
   AM_Sizer.AddSpacing( 12 );
   AM_Sizer.Add( M_NumericEdit );
   AM_Sizer.AddStretch();

   //

   Q_NumericEdit.label.SetStyleSheet( lblStyleSheet_i );
   Q_NumericEdit.label.SetText( u"q\u2009" );
   Q_NumericEdit.SetReal();
   Q_NumericEdit.EnableFixedPrecision();
   Q_NumericEdit.SetRange( TheEGElemQParameter->MinimumValue(), TheEGElemQParameter->MaximumValue() );
   Q_NumericEdit.SetPrecision( TheEGElemQParameter->Precision() );
   Q_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   Q_NumericEdit.SetToolTip( "<p>Perihelion distance at the epoch (au).</p>" );
   Q_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   T_NumericEdit.label.SetStyleSheet( lblStyleSheet_i );
   T_NumericEdit.label.SetText( u"T\u2009" );
   T_NumericEdit.SetReal();
   T_NumericEdit.EnableFixedPrecision();
   T_NumericEdit.SetRange( TheEGElemTParameter->MinimumValue(), TheEGElemTParameter->MaximumValue() );
   T_NumericEdit.SetPrecision( TheEGElemTParameter->Precision() );
   T_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   T_NumericEdit.SetToolTip( "<p>Time of perihelion passage (JD, TDB).</p>" );
   T_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   QT_Sizer.Add( Q_NumericEdit );
   QT_Sizer.AddSpacing( 12 );
   QT_Sizer.Add( T_NumericEdit );
   QT_Sizer.AddStretch();

   //

   E_NumericEdit.label.SetStyleSheet( lblStyleSheet_i );
   E_NumericEdit.label.SetText( u"e\u2009" );
   E_NumericEdit.SetReal();
   E_NumericEdit.EnableFixedPrecision();
   E_NumericEdit.SetRange( TheEGElemEParameter->MinimumValue(), TheEGElemEParameter->MaximumValue() );
   E_NumericEdit.SetPrecision( TheEGElemEParameter->Precision() );
   E_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   E_NumericEdit.SetToolTip( "<p>Orbital eccentricity.</p>" );
   E_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   O_NumericEdit.label.SetStyleSheet( lblStyleSheet_n );
   O_NumericEdit.label.SetText( u"\u03A9\u2009" );
   O_NumericEdit.SetReal();
   O_NumericEdit.EnableFixedPrecision();
   O_NumericEdit.SetRange( TheEGElemOParameter->MinimumValue(), TheEGElemOParameter->MaximumValue() );
   O_NumericEdit.SetPrecision( TheEGElemOParameter->Precision() );
   O_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   O_NumericEdit.SetToolTip( "<p>Longitude of the ascending node (degrees).</p>" );
   O_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   EO_Sizer.Add( E_NumericEdit );
   EO_Sizer.AddSpacing( 12 );
   EO_Sizer.Add( O_NumericEdit );
   EO_Sizer.AddStretch();

   //

   I_NumericEdit.label.SetStyleSheet( lblStyleSheet_i );
   I_NumericEdit.label.SetText( u"i\u2009" );
   I_NumericEdit.SetReal();
   I_NumericEdit.EnableFixedPrecision();
   I_NumericEdit.SetRange( TheEGElemIParameter->MinimumValue(), TheEGElemIParameter->MaximumValue() );
   I_NumericEdit.SetPrecision( TheEGElemIParameter->Precision() );
   I_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   I_NumericEdit.SetToolTip( "<p>Orbit inclination over the fundamental plane (degrees).</p>" );
   I_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   W_NumericEdit.label.SetStyleSheet( lblStyleSheet_i );
   W_NumericEdit.label.SetText( u"\u03C9\u2009" );
   W_NumericEdit.SetReal();
   W_NumericEdit.EnableFixedPrecision();
   W_NumericEdit.SetRange( TheEGElemWParameter->MinimumValue(), TheEGElemWParameter->MaximumValue() );
   W_NumericEdit.SetPrecision( TheEGElemWParameter->Precision() );
   W_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   W_NumericEdit.SetToolTip( "<p>Argument of perihelion (degrees).</p>" );
   W_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   ToVectors_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/arrows/arrow-up.png" ) ) );
   ToVectors_ToolButton.SetScaledFixedSize( 20, 20 );
   ToVectors_ToolButton.SetToolTip( "<p>Compute state vectors from classical orbital elements.</p>" );
   ToVectors_ToolButton.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   IW_Sizer.Add( I_NumericEdit );
   IW_Sizer.AddSpacing( 12 );
   IW_Sizer.Add( W_NumericEdit );
   IW_Sizer.AddSpacing( 12 );
   IW_Sizer.Add( ToVectors_ToolButton );
   IW_Sizer.AddStretch();

   //

   OrbitalElements_Sizer.SetMargin( 8 );
   OrbitalElements_Sizer.SetSpacing( 8 );
   OrbitalElements_Sizer.Add( AM_Sizer );
   OrbitalElements_Sizer.Add( QT_Sizer );
   OrbitalElements_Sizer.Add( EO_Sizer );
   OrbitalElements_Sizer.Add( IW_Sizer );

   OrbitalElements_GroupBox.SetSizer( OrbitalElements_Sizer );

   //

   const char* epochToolTip = "<p>The date (in the TDB timescale) at which the specified initial coordinates "
                        "or orbital elements are valid. This date will be the starting time for numerical "
                        "integration of orbital motion.</p>";

   Epoch_Label.SetText( "Epoch:" );
   Epoch_Label.SetFixedWidth( labelWidth1 );
   Epoch_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Epoch_Label.SetToolTip( epochToolTip );

   Epoch_Edit.SetToolTip( epochToolTip );
   Epoch_Edit.SetStyleSheet( crdStyleSheet );
   Epoch_Edit.OnEditCompleted( (Edit::edit_event_handler)&EphemerisGeneratorInterface::e_TimeEditCompleted, w );

   Epoch_Sizer.SetSpacing( 4 );
   Epoch_Sizer.Add( Epoch_Label );
   Epoch_Sizer.Add( Epoch_Edit, 100 );
   Epoch_Sizer.AddStretch();

   //

   const char* objectIdToolTip = "<p>Identifier of the object. For example, an asteroid number such as '5' for 5 Astraea.</p>"
      "<p><b>Important:</b> If you are integrating the orbit of an asteroid already included in the selected asteroid "
      "ephemerides file, you should specify its identifier or name in that file. In this way the numerical integration process "
      "will be able to exclude the object as a perturber body. Otherwise the integration will detect a collision between the "
      "object being integrated and the same object being used as a perturber.</p>";

   ObjectId_Label.SetText( "Object id:" );
   ObjectId_Label.SetFixedWidth( labelWidth1 );
   ObjectId_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ObjectId_Label.SetToolTip( objectIdToolTip );

   ObjectId_Edit.SetToolTip( objectIdToolTip );
   ObjectId_Edit.SetStyleSheet( crdStyleSheet );
   ObjectId_Edit.OnEditCompleted( (Edit::edit_event_handler)&EphemerisGeneratorInterface::e_IdEditCompleted, w );

   ObjectId_Sizer.SetSpacing( 4 );
   ObjectId_Sizer.Add( ObjectId_Label );
   ObjectId_Sizer.Add( ObjectId_Edit, 100 );
   ObjectId_Sizer.AddStretch();

   //

   const char* objectNameToolTip = "<p>Name of the object. For example, an asteroid name such as 'Astraea' for 5 Astraea, "
      "the designation of an unnumbered minor planet such as '2006 QH181', or a comet name such as '1/P Halley'.</p>"
      "<p><b>Important:</b> If you are integrating the orbit of an asteroid already included in the selected asteroid "
      "ephemerides file, you should specify its identifier or name in that file. In this way the numerical integration process "
      "will be able to exclude the object as a perturber body. Otherwise the integration will detect a collision between the "
      "object being integrated and the same object being used as a perturber.</p>";

   ObjectName_Label.SetText( "Object name:" );
   ObjectName_Label.SetFixedWidth( labelWidth1 );
   ObjectName_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ObjectName_Label.SetToolTip( objectNameToolTip );

   ObjectName_Edit.SetToolTip( objectNameToolTip );
   ObjectName_Edit.SetStyleSheet( crdStyleSheet );
   ObjectName_Edit.OnEditCompleted( (Edit::edit_event_handler)&EphemerisGeneratorInterface::e_IdEditCompleted, w );

   ObjectName_Sizer.SetSpacing( 4 );
   ObjectName_Sizer.Add( ObjectName_Label );
   ObjectName_Sizer.Add( ObjectName_Edit, 100 );
   ObjectName_Sizer.AddStretch();

   //

   H_NumericEdit.label.SetText( "H:" );
   H_NumericEdit.label.SetFixedWidth( labelWidth1 );
   H_NumericEdit.SetReal();
   H_NumericEdit.EnableFixedPrecision();
   H_NumericEdit.SetRange( TheEGHParameter->MinimumValue(), TheEGHParameter->MaximumValue() );
   H_NumericEdit.SetPrecision( TheEGHParameter->Precision() );
   H_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   H_NumericEdit.sizer.AddStretch();
   H_NumericEdit.SetToolTip( "<p>Absolute magnitude.</p>" );
   H_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   B_V_CheckBox.SetText( "" );
   B_V_CheckBox.SetToolTip( "<p>Checked if the B-V color index is available.</p>" );
   B_V_CheckBox.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   B_V_NumericEdit.label.SetText( "B-V:" );
   B_V_NumericEdit.label.SetFixedWidth( labelWidth2 );
   B_V_NumericEdit.SetReal();
   B_V_NumericEdit.EnableFixedPrecision();
   B_V_NumericEdit.SetRange( TheEGB_VParameter->MinimumValue(), TheEGB_VParameter->MaximumValue() );
   B_V_NumericEdit.SetPrecision( TheEGB_VParameter->Precision() );
   B_V_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   B_V_NumericEdit.SetToolTip( "<p>B-V color index (mag).</p>" );
   B_V_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   H_Sizer.Add( H_NumericEdit );
   H_Sizer.AddSpacing( 12 );
   H_Sizer.Add( B_V_CheckBox );
   H_Sizer.Add( B_V_NumericEdit );
   H_Sizer.AddStretch();

   //

   G_NumericEdit.label.SetText( "G:" );
   G_NumericEdit.label.SetFixedWidth( labelWidth1 );
   G_NumericEdit.SetReal();
   G_NumericEdit.EnableFixedPrecision();
   G_NumericEdit.SetRange( TheEGGParameter->MinimumValue(), TheEGGParameter->MaximumValue() );
   G_NumericEdit.SetPrecision( TheEGGParameter->Precision() );
   G_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   G_NumericEdit.sizer.AddStretch();
   G_NumericEdit.SetToolTip( "<p>Slope parameter.</p>" );
   G_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   D_CheckBox.SetText( "" );
   D_CheckBox.SetToolTip( "<p>Checked if the object's diameter is available.</p>" );
   D_CheckBox.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   D_NumericEdit.label.SetText( "D:" );
   D_NumericEdit.label.SetFixedWidth( labelWidth2 );
   D_NumericEdit.SetReal();
   D_NumericEdit.EnableFixedPrecision();
   D_NumericEdit.SetRange( TheEGDParameter->MinimumValue(), TheEGDParameter->MaximumValue() );
   D_NumericEdit.SetPrecision( TheEGDParameter->Precision() );
   D_NumericEdit.edit.SetStyleSheet( crdStyleSheet );
   D_NumericEdit.SetToolTip( "<p>Object's mean diameter (km).</p>" );
   D_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&EphemerisGeneratorInterface::e_EditValueUpdated, w );

   G_Sizer.Add( G_NumericEdit );
   G_Sizer.AddSpacing( 12 );
   G_Sizer.Add( D_CheckBox );
   G_Sizer.Add( D_NumericEdit );
   G_Sizer.AddStretch();

   //

   ObjectParameters_Sizer.SetSpacing( 4 );
   ObjectParameters_Sizer.Add( Epoch_Sizer );
   ObjectParameters_Sizer.Add( ObjectId_Sizer );
   ObjectParameters_Sizer.Add( ObjectName_Sizer );
   ObjectParameters_Sizer.Add( H_Sizer );
   ObjectParameters_Sizer.Add( G_Sizer );

   ObjectParameters_Control.SetSizer( ObjectParameters_Sizer );

   //

   Database_GroupBox.SetTitle( "Database" );
   Database_GroupBox.EnableTitleCheckBox();
   Database_GroupBox.OnCheck( (GroupBox::check_event_handler)&EphemerisGeneratorInterface::e_GroupBoxCheck, w );

   //

   const char* databasePathToolTip = "<p>Path to a database file of orbital elements or state vectors, where database search "
      "operations will be performed.</p>"
      "<p>Currently we support plain text databases in a variety of formats, including well-known public databases of "
      "asteroid and comet orbital elements available from Lowell Observatory, MPC and JPL, as well as several CSV and tabular "
      "formats generated by our scripts and tools. See the <i>database format</i> parameter below.</p>";

   DatabasePath_Label.SetText( "Database file:" );
   DatabasePath_Label.SetFixedWidth( labelWidth1-ui9 );
   DatabasePath_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DatabasePath_Label.SetToolTip( databasePathToolTip );

   DatabasePath_Edit.SetToolTip( databasePathToolTip );
   DatabasePath_Edit.SetScaledMinWidth( 250 );
   DatabasePath_Edit.OnEditCompleted( (Edit::edit_event_handler)&EphemerisGeneratorInterface::e_FilePathEditCompleted, w );

   DatabasePath_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select-file.png" ) ) );
   DatabasePath_ToolButton.SetScaledFixedSize( 20, 20 );
   DatabasePath_ToolButton.SetToolTip( "<p>Select an object database file.</p>" );
   DatabasePath_ToolButton.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   DatabasePath_Sizer.SetSpacing( 4 );
   DatabasePath_Sizer.Add( DatabasePath_Label );
   DatabasePath_Sizer.Add( DatabasePath_Edit, 100 );
   DatabasePath_Sizer.Add( DatabasePath_ToolButton );

   //

   const char* databaseFormatToolTip = "<p>We support plain text databases or orbital elements and state vectors in a variety "
      "of formats, including well-known public databases of asteroid and comet orbital elements available from Lowell Observatory, "
      "MPC and JPL, as well as several CSV and tabular formats generated by our scripts and tools.</p>"
      "<p>Supported plain text database formats are described in a special XML file included in the standard PixInsight "
      "distribution. This means that new formats can be added easily without needing to release a new version of the "
      "EphemerisGenerator process. Please feel free to let us know about additional databases that you'd like us to implement.</p>";

   DatabaseFormat_Label.SetText( "Database format:" );
   DatabaseFormat_Label.SetFixedWidth( labelWidth1-ui9 );
   DatabaseFormat_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DatabaseFormat_Label.SetToolTip( databaseFormatToolTip );

   EphemerisGeneratorProcess::InitializeDatabaseFormats();
   for ( const auto& format : EphemerisGeneratorProcess::DatabaseFormats() )
      DatabaseFormat_ComboBox.AddItem( format.name );
   DatabaseFormat_ComboBox.SetToolTip( databaseFormatToolTip );
   DatabaseFormat_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&EphemerisGeneratorInterface::e_ItemSelected, w );

   DatabaseFormat_Sizer.SetSpacing( 4 );
   DatabaseFormat_Sizer.Add( DatabaseFormat_Label );
   DatabaseFormat_Sizer.Add( DatabaseFormat_ComboBox );
   DatabaseFormat_Sizer.AddStretch();

   //

   const char* objectsToolTip = "<p>A comma-separated list of search tokens or regular expressions that will be used to "
      "select a subset of database objects for numerical integration and ephemeris generation.</p>"
      "<p>Each item in the comma-separated list will be interpreted either as a regular expression (ECMAScript format, "
      "case-insensitive) if the <i>Regular expressions</i> option is selected, or as a literal search token otherwise.</p>"
      "<p><b>Regular Expressions</b></p>"
      "<p>Regular expressions will be applied to object identifiers and names. Only those objects with identifiers or "
      "names that match one of the regular expressions will be included in the search result.</p>"
      "<p>For example, to select a list of objects given by their asteroid numbers from an asteroid database (such as "
      "Lowell's astorb.dat for example), the following list could be used:</p>"
      "<p style=\"margin-left:2em;\">10,40,41,127</p>"
      "<p>This would select the following asteroids: 10 Hygiea, 40 Harmonia, 41 Daphne, 127 Johanna. The following list "
      "would produce exactly the same result:</p>"
      "<p style=\"margin-left:2em;\">10,Harmonia,41,127</p>"
      "<p>since regular expressions are matched against object identifiers and names. Expression matching is case-insensitive, "
      "so 'harmonia' would also work in this case.</p>"
      "<p>The following regular expression will select the asteroids with numbers from 100 to 195:</p>"
      "<p style=\"margin-left:2em;\">1[0-9][0-5]</p>"
      "<p>This one will select unnamed asteroids whose designations start with '2020 ':</p>"
      "<p style=\"margin-left:2em;\">2020\\s.+</p>"
      "<p>For a good online tutorial on regular expressions, check regexone.com.</p>"
      "<p><b>Search Ranges</b></p>"
      "<p>When the <i>Regular expressions</i> option is disabled, search tokens can specify identifier ranges with the form:</p>"
      "<p style=\"margin-left:2em;\">n1-&gt;n2</p>"
      "<p>where n1 and n2 are two integer numbers. Only the set of asteroids with numbers within the range [n1,n2] will be "
      "selected. For example:</p>"
      "<p style=\"margin-left:2em;\">125-&gt;500,1000-&gt;1500</p>"
      "<p>will select the numbered asteroids from 125 to 500 and from 1000 to 1500.</p>"
      "<p><b>Literal Search Tokens</b></p>"
      "<p>When the <i>Regular expressions</i> option is disabled and a search token does not specify a search range, it "
      "will be interpreted as a case-insensitive substring that will be searched for literally in object identifiers and "
      "names. For example, the following search tokens:</p>"
      "<p style=\"margin-left:2em;\">shoemaker-levy 9</p>"
      "<p>would select all fragments of comet D/1993 F2 Shoemaker-Levy 9 (21 objects).</p>";

   DatabaseObjects_Label.SetText( "Search tokens:" );
   DatabaseObjects_Label.SetFixedWidth( labelWidth1-ui9 );
   DatabaseObjects_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DatabaseObjects_Label.SetToolTip( objectsToolTip );

   DatabaseObjects_TextBox.SetScaledMinWidth( 250 );
   DatabaseObjects_TextBox.SetScaledMaxHeight( 40 );
   DatabaseObjects_TextBox.SetStyleSheet( crdStyleSheet );
   DatabaseObjects_TextBox.SetToolTip( objectsToolTip );
   DatabaseObjects_TextBox.OnTextUpdated( (TextBox::unicode_event_handler)&EphemerisGeneratorInterface::e_TextUpdated, w );

   DatabaseObjects_Sizer.SetSpacing( 4 );
   DatabaseObjects_Sizer.Add( DatabaseObjects_Label );
   DatabaseObjects_Sizer.Add( DatabaseObjects_TextBox, 100 );

   //

   UseRegularExpressions_CheckBox.SetText( "Regular expressions" );
   UseRegularExpressions_CheckBox.SetToolTip( "<p>If checked, the <i>Search tokens</i> parameter will be interpreted as a "
      "comma-separated list of regular expressions (ECMAScript format, case-insensitive). Only objects with "
      "identifiers or names that match one of the specified regular expressions will be included in the search result.</p>"
      "<p>If this option is disabled, the contents of the <i>Search tokens</i> parameter is a comma-separated list of literal "
      "search tokens and/or asteroid identifier ranges; see the documentation for that parameter for more information.</p>" );
   UseRegularExpressions_CheckBox.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   ExcludeCoreAsteroids_CheckBox.SetText( "Exclude core asteroids" );
   ExcludeCoreAsteroids_CheckBox.SetToolTip( "<p>Exclude asteroids with identifiers found in core ephemerides "
      "XEPH files from database search results. For example, if this option is enabled, '1 Ceres' would not be "
      "included in any search result under normal conditions.</p>"
      "<p>This option is useful to avoid duplicating already available objects in newly generated XEPH files.</p>" );
   ExcludeCoreAsteroids_CheckBox.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   DatabaseOptions_Sizer.AddUnscaledSpacing( labelWidth1-ui9+ui4 );
   DatabaseOptions_Sizer.Add( UseRegularExpressions_CheckBox );
   DatabaseOptions_Sizer.AddSpacing( 8 );
   DatabaseOptions_Sizer.Add( ExcludeCoreAsteroids_CheckBox );
   DatabaseOptions_Sizer.AddStretch();

   //

   GetFirst_Button.SetText( "Get First" );
   GetFirst_Button.SetIcon( w.ScaledResource( ":/icons/arrow-top.png" ) );
   GetFirst_Button.SetStyleSheet( "QPushButton { text-align: left; }" );
   GetFirst_Button.SetToolTip( "<p>Retrieve data for the first object found in the specified database, "
      "then populate the state vectors or orbital elements (depending on database contents), as well as "
      "photometric and physical object properties.</p>" );
   GetFirst_Button.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   CheckObjects_Button.SetText( "Check Objects" );
   CheckObjects_Button.SetIcon( w.ScaledResource( ":/icons/document-text.png" ) );
   CheckObjects_Button.SetStyleSheet( "QPushButton { text-align: left; }" );
   CheckObjects_Button.SetToolTip( "<p>Retrieve data for all selected objects and present it as a text document.</p>" );
   CheckObjects_Button.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   DatabaseButtons_Sizer.SetSpacing( 8 );
   DatabaseButtons_Sizer.AddUnscaledSpacing( labelWidth1-ui9+ui4 );
   DatabaseButtons_Sizer.Add( GetFirst_Button );
   DatabaseButtons_Sizer.Add( CheckObjects_Button );
   DatabaseButtons_Sizer.AddStretch();

   //

   Database_Sizer.SetMargin( 8 );
   Database_Sizer.SetSpacing( 4 );
   Database_Sizer.Add( DatabasePath_Sizer );
   Database_Sizer.Add( DatabaseFormat_Sizer );
   Database_Sizer.Add( DatabaseObjects_Sizer );
   Database_Sizer.Add( DatabaseOptions_Sizer );
   Database_Sizer.Add( DatabaseButtons_Sizer );

   Database_GroupBox.SetSizer( Database_Sizer );

   //

   InitialConditions_Sizer.SetSpacing( 4 );
   InitialConditions_Sizer.Add( WorkingMode_Sizer );
   InitialConditions_Sizer.Add( StateVectors_GroupBox );
   InitialConditions_Sizer.Add( OrbitalElements_GroupBox );
   InitialConditions_Sizer.Add( ObjectParameters_Control );
   InitialConditions_Sizer.Add( Database_GroupBox );

   InitialConditions_Control.SetSizer( InitialConditions_Sizer );

   //

   NumericalIntegration_SectionBar.SetTitle( "Numerical Integration" );
   NumericalIntegration_SectionBar.SetSection( NumericalIntegration_Control );
   NumericalIntegration_SectionBar.SetToolTip( "<p>Working parameters for the numerical integration process.</p>" );

   //

   const char* startTimeToolTip = "<p>The starting time for numerical integration in the TDB timescale.</p>";

   StartTime_Label.SetText( "Start time:" );
   StartTime_Label.SetFixedWidth( labelWidth1 );
   StartTime_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   StartTime_Label.SetToolTip( startTimeToolTip );

   StartTime_Edit.SetToolTip( startTimeToolTip );
   StartTime_Edit.SetStyleSheet( crdStyleSheet );
   StartTime_Edit.OnEditCompleted( (Edit::edit_event_handler)&EphemerisGeneratorInterface::e_TimeEditCompleted, w );

   StartTime_Sizer.SetSpacing( 4 );
   StartTime_Sizer.Add( StartTime_Label );
   StartTime_Sizer.Add( StartTime_Edit, 100 );
   StartTime_Sizer.AddStretch();

   //

   const char* endTimeToolTip = "<p>The final time for numerical integration in the TDB timescale.</p>";

   EndTime_Label.SetText( "End time:" );
   EndTime_Label.SetFixedWidth( labelWidth1 );
   EndTime_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   EndTime_Label.SetToolTip( endTimeToolTip );

   EndTime_Edit.SetToolTip( endTimeToolTip );
   EndTime_Edit.SetStyleSheet( crdStyleSheet );
   EndTime_Edit.OnEditCompleted( (Edit::edit_event_handler)&EphemerisGeneratorInterface::e_TimeEditCompleted, w );

   EndTime_Sizer.SetSpacing( 4 );
   EndTime_Sizer.Add( EndTime_Label );
   EndTime_Sizer.Add( EndTime_Edit, 100 );
   EndTime_Sizer.AddStretch();

   //

   const char* fundamentalEphemeridesToolTip = "<p>The XEPH file providing fundamental solar system ephemerides.</p>"
                        "<p>If this field is left blank (or with its default &lt;core default&gt; value), "
                        "the default core fundamental ephemerides file will be used.</p>";

   FundamentalEphemerides_Label.SetText( "Fundamental ephemerides:" );
   FundamentalEphemerides_Label.SetFixedWidth( labelWidth1 );
   FundamentalEphemerides_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   FundamentalEphemerides_Label.SetToolTip( fundamentalEphemeridesToolTip );

   FundamentalEphemerides_Edit.SetToolTip( fundamentalEphemeridesToolTip );
   FundamentalEphemerides_Edit.SetScaledMinWidth( 250 );
   FundamentalEphemerides_Edit.OnEditCompleted( (Edit::edit_event_handler)&EphemerisGeneratorInterface::e_FilePathEditCompleted, w );
   FundamentalEphemerides_Edit.OnGetFocus( (Control::event_handler)&EphemerisGeneratorInterface::e_FilePathEditGetFocus, w );
   FundamentalEphemerides_Edit.OnLoseFocus( (Control::event_handler)&EphemerisGeneratorInterface::e_FilePathEditLoseFocus, w );

   FundamentalEphemerides_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select-file.png" ) ) );
   FundamentalEphemerides_ToolButton.SetScaledFixedSize( 20, 20 );
   FundamentalEphemerides_ToolButton.SetToolTip( "<p>Select the fundamental ephemerides XEPH file.</p>" );
   FundamentalEphemerides_ToolButton.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   FundamentalEphemerides_Sizer.SetSpacing( 4 );
   FundamentalEphemerides_Sizer.Add( FundamentalEphemerides_Label );
   FundamentalEphemerides_Sizer.Add( FundamentalEphemerides_Edit, 100 );
   FundamentalEphemerides_Sizer.Add( FundamentalEphemerides_ToolButton );

   //

   AsteroidPerturbers_CheckBox.SetText( "Asteroid perturbers" );
   AsteroidPerturbers_CheckBox.SetToolTip( "<p>Include the most massive asteroids as individual perturber bodies.</p>"
                        "<p>In current versions of PixInsight this includes the complete set of 343 asteroids used "
                        "for the numerical integration of JPL's DE430 ephemerides.</p>" );
   AsteroidPerturbers_CheckBox.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   AsteroidPerturbers_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   AsteroidPerturbers_Sizer.Add( AsteroidPerturbers_CheckBox );

   //

   const char* asteroidEphemeridesToolTip = "<p>The XEPH file providing massive asteroid ephemerides.</p>"
                        "<p>If this field is left blank (or with its default &lt;core default&gt; value), "
                        "the default core asteroid ephemerides file will be used.</p>"
                        "<p>This file will only be used (and required) when the <i>Asteroid perturbers</i> "
                        "option is selected.</p>"
                        "<b>Important: </b> The selected fundamental and asteroid ephemerides files must be "
                        "coherent, that is, both must be referred to the same dynamical solar system model. "
                        "For example, you should not mix DE440 fundamental ephemerides with asteroids referred "
                        "to DE430 barycentric solar positions. Under normal working conditions, this will never "
                        "be a problem if the default core ephemerides files are selected.</p>";

   AsteroidEphemerides_Label.SetText( "Asteroid ephemerides:" );
   AsteroidEphemerides_Label.SetFixedWidth( labelWidth1 );
   AsteroidEphemerides_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   AsteroidEphemerides_Label.SetToolTip( asteroidEphemeridesToolTip );

   AsteroidEphemerides_Edit.SetToolTip( asteroidEphemeridesToolTip );
   AsteroidEphemerides_Edit.SetScaledMinWidth( 250 );
   AsteroidEphemerides_Edit.OnEditCompleted( (Edit::edit_event_handler)&EphemerisGeneratorInterface::e_FilePathEditCompleted, w );
   AsteroidEphemerides_Edit.OnGetFocus( (Control::event_handler)&EphemerisGeneratorInterface::e_FilePathEditGetFocus, w );
   AsteroidEphemerides_Edit.OnLoseFocus( (Control::event_handler)&EphemerisGeneratorInterface::e_FilePathEditLoseFocus, w );

   AsteroidEphemerides_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select-file.png" ) ) );
   AsteroidEphemerides_ToolButton.SetScaledFixedSize( 20, 20 );
   AsteroidEphemerides_ToolButton.SetToolTip( "<p>Select the asteroid ephemerides XEPH file.</p>" );
   AsteroidEphemerides_ToolButton.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   AsteroidEphemerides_Sizer.SetSpacing( 4 );
   AsteroidEphemerides_Sizer.Add( AsteroidEphemerides_Label );
   AsteroidEphemerides_Sizer.Add( AsteroidEphemerides_Edit, 100 );
   AsteroidEphemerides_Sizer.Add( AsteroidEphemerides_ToolButton );

   //

   KBOPerturbers_CheckBox.SetText( "KBO perturbers" );
   KBOPerturbers_CheckBox.SetToolTip( "<p>Include massive Kuiper belt objects (KBOs) as individual perturber bodies.</p>"
                        "<p>In current versions of PixInsight this includes the 30 most massive KBOs used for the "
                        "numerical integration of JPL's DE440 ephemerides.</p>" );
   KBOPerturbers_CheckBox.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   KBOPerturbers_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   KBOPerturbers_Sizer.Add( KBOPerturbers_CheckBox );

   //

   const char* kboEphemeridesToolTip = "<p>The XEPH file providing massive KBO ephemerides.</p>"
                        "<p>This file will only be used (and required) when the <i>KBO perturbers</i> "
                        "option is selected.</p>";

   KBOEphemerides_Label.SetText( "KBO ephemerides:" );
   KBOEphemerides_Label.SetFixedWidth( labelWidth1 );
   KBOEphemerides_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   KBOEphemerides_Label.SetToolTip( kboEphemeridesToolTip );

   KBOEphemerides_Edit.SetToolTip( kboEphemeridesToolTip );
   KBOEphemerides_Edit.SetScaledMinWidth( 250 );
   KBOEphemerides_Edit.OnEditCompleted( (Edit::edit_event_handler)&EphemerisGeneratorInterface::e_FilePathEditCompleted, w );

   KBOEphemerides_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select-file.png" ) ) );
   KBOEphemerides_ToolButton.SetScaledFixedSize( 20, 20 );
   KBOEphemerides_ToolButton.SetToolTip( "<p>Select the KBO ephemerides XEPH file.</p>" );
   KBOEphemerides_ToolButton.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   KBOEphemerides_Sizer.SetSpacing( 4 );
   KBOEphemerides_Sizer.Add( KBOEphemerides_Label );
   KBOEphemerides_Sizer.Add( KBOEphemerides_Edit, 100 );
   KBOEphemerides_Sizer.Add( KBOEphemerides_ToolButton );

   //

//    SeparateEarthMoon_CheckBox.SetText( "Separate Earth and Moon" );
//    SeparateEarthMoon_CheckBox.SetToolTip( "<p>Consider Earth and Moon as two separate perturber bodies.</p>"
//                         "<p>If disabled, a combined Earth-Moon perturber with Earth-Moon barycenter coordinates "
//                         "and combined mass parameter will be used instead.</p>"
//                         "<p>Under normal working conditions this option should always be enabled for the sake "
//                         "of accuracy in computed ephemerides. It can be disabled for testing purposes.</p>" );
//    SeparateEarthMoon_CheckBox.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );
//
//    SeparateEarthMoon_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
//    SeparateEarthMoon_Sizer.Add( SeparateEarthMoon_CheckBox );

   //

   NumericalIntegration_Sizer.SetSpacing( 4 );
   NumericalIntegration_Sizer.Add( StartTime_Sizer );
   NumericalIntegration_Sizer.Add( EndTime_Sizer );
   NumericalIntegration_Sizer.Add( FundamentalEphemerides_Sizer );
   NumericalIntegration_Sizer.Add( AsteroidPerturbers_Sizer );
   NumericalIntegration_Sizer.Add( AsteroidEphemerides_Sizer );
   NumericalIntegration_Sizer.Add( KBOPerturbers_Sizer );
   NumericalIntegration_Sizer.Add( KBOEphemerides_Sizer );
//    NumericalIntegration_Sizer.Add( SeparateEarthMoon_Sizer );

   NumericalIntegration_Control.SetSizer( NumericalIntegration_Sizer );

   //

   Output_SectionBar.SetTitle( "Output" );
   Output_SectionBar.SetSection( Output_Control );
   Output_SectionBar.SetToolTip( "<p>Output data and parameters.</p>" );

   //

   OutputXEPHFile_CheckBox.SetText( "Generate ephemerides file" );
   OutputXEPHFile_CheckBox.SetToolTip( "<p>Generate an output ephemerides file in XEPH format.</p>" );
   OutputXEPHFile_CheckBox.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   OutputXEPHFile_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OutputXEPHFile_Sizer.Add( OutputXEPHFile_CheckBox );

   //

   const char* outputXEPHFilePathToolTip = "<p>The path to the output XEPH file.</p>"
      "<p>Custom XEPH files generated with this tool can be used to compute ephemerides for a wide range of "
      "applications on the PixInsight PCL/PJSR platforms. For example, you can use them with the AnnotateImage script "
      "to find and annotate asteroids and comets in your images, or with the Ephemerides script to compute positional "
      "and visibility data in a variety of formats and conditions.</p>";

   OutputXEPHFilePath_Label.SetText( "Output XEPH file:" );
   OutputXEPHFilePath_Label.SetFixedWidth( labelWidth1 );
   OutputXEPHFilePath_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputXEPHFilePath_Label.SetToolTip( outputXEPHFilePathToolTip );

   OutputXEPHFilePath_Edit.SetToolTip( outputXEPHFilePathToolTip );
   OutputXEPHFilePath_Edit.SetScaledMinWidth( 250 );
   OutputXEPHFilePath_Edit.OnEditCompleted( (Edit::edit_event_handler)&EphemerisGeneratorInterface::e_FilePathEditCompleted, w );
   OutputXEPHFilePath_Edit.OnGetFocus( (Control::event_handler)&EphemerisGeneratorInterface::e_FilePathEditGetFocus, w );
   OutputXEPHFilePath_Edit.OnLoseFocus( (Control::event_handler)&EphemerisGeneratorInterface::e_FilePathEditLoseFocus, w );

   OutputXEPHFilePath_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/browser/select-file.png" ) ) );
   OutputXEPHFilePath_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputXEPHFilePath_ToolButton.SetToolTip( "<p>Select the output XEPH file.</p>" );
   OutputXEPHFilePath_ToolButton.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   OutputXEPHFilePath_Sizer.SetSpacing( 4 );
   OutputXEPHFilePath_Sizer.Add( OutputXEPHFilePath_Label );
   OutputXEPHFilePath_Sizer.Add( OutputXEPHFilePath_Edit, 100 );
   OutputXEPHFilePath_Sizer.Add( OutputXEPHFilePath_ToolButton );

   //

   OverwriteExistingFiles_CheckBox.SetText( "Overwrite existing files" );
   OverwriteExistingFiles_CheckBox.SetToolTip( "<p>If this option is selected, EphemerisGenerator will overwrite "
      "existing .xeph files with the same names as generated output files. This can be dangerous because the original "
      "contents of overwritten files will be lost.</p>"
      "<p><b>Enable this option <u>at your own risk.</u></b></p>" );
   OverwriteExistingFiles_CheckBox.OnClick( (Button::click_event_handler)&EphemerisGeneratorInterface::e_Click, w );

   OverwriteExistingFiles_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OverwriteExistingFiles_Sizer.Add( OverwriteExistingFiles_CheckBox );

   //

   Output_Sizer.SetSpacing( 4 );
   Output_Sizer.Add( OutputXEPHFile_Sizer );
   Output_Sizer.Add( OutputXEPHFilePath_Sizer );
   Output_Sizer.Add( OverwriteExistingFiles_Sizer );

   Output_Control.SetSizer( Output_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( InitialConditions_SectionBar );
   Global_Sizer.Add( InitialConditions_Control );
   Global_Sizer.Add( NumericalIntegration_SectionBar );
   Global_Sizer.Add( NumericalIntegration_Control );
   Global_Sizer.Add( Output_SectionBar );
   Global_Sizer.Add( Output_Control );

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();

   int crdEditWidth = X_NumericEdit.edit.Font().Width( String( '0', 24 ) );

   X_NumericEdit.edit.SetFixedWidth( crdEditWidth );
   Y_NumericEdit.edit.SetFixedWidth( crdEditWidth );
   Z_NumericEdit.edit.SetFixedWidth( crdEditWidth );
   XDot_NumericEdit.edit.SetFixedWidth( crdEditWidth );
   YDot_NumericEdit.edit.SetFixedWidth( crdEditWidth );
   ZDot_NumericEdit.edit.SetFixedWidth( crdEditWidth );

   A_NumericEdit.edit.SetFixedWidth( crdEditWidth );
   M_NumericEdit.edit.SetFixedWidth( crdEditWidth );
   Q_NumericEdit.edit.SetFixedWidth( crdEditWidth );
   T_NumericEdit.edit.SetFixedWidth( crdEditWidth );
   E_NumericEdit.edit.SetFixedWidth( crdEditWidth );
   O_NumericEdit.edit.SetFixedWidth( crdEditWidth );
   I_NumericEdit.edit.SetFixedWidth( crdEditWidth );
   W_NumericEdit.edit.SetFixedWidth( crdEditWidth );

   int crdLabelWidth = X_NumericEdit.label.Font().Width( "XX" );

   X_NumericEdit.label.SetFixedWidth( crdLabelWidth );
   XDot_NumericEdit.label.SetFixedWidth( crdLabelWidth );
   Y_NumericEdit.label.SetFixedWidth( crdLabelWidth );
   YDot_NumericEdit.label.SetFixedWidth( crdLabelWidth );
   Z_NumericEdit.label.SetFixedWidth( crdLabelWidth );
   ZDot_NumericEdit.label.SetFixedWidth( crdLabelWidth );
   A_NumericEdit.label.SetFixedWidth( crdLabelWidth );
   M_NumericEdit.label.SetFixedWidth( crdLabelWidth );
   Q_NumericEdit.label.SetFixedWidth( crdLabelWidth );
   T_NumericEdit.label.SetFixedWidth( crdLabelWidth );
   E_NumericEdit.label.SetFixedWidth( crdLabelWidth );
   O_NumericEdit.label.SetFixedWidth( crdLabelWidth );
   I_NumericEdit.label.SetFixedWidth( crdLabelWidth );
   W_NumericEdit.label.SetFixedWidth( crdLabelWidth );

   int timeEditWidth = Epoch_Edit.Font().Width( "0000-00-00T00:00:00.000XXX" );

   Epoch_Edit.SetFixedWidth( timeEditWidth );
   ObjectId_Edit.SetFixedWidth( timeEditWidth );
   ObjectName_Edit.SetFixedWidth( timeEditWidth );
   StartTime_Edit.SetFixedWidth( timeEditWidth );
   EndTime_Edit.SetFixedWidth( timeEditWidth );

   int hgEditWidth = X_NumericEdit.edit.Font().Width( String( '0', 8 ) );

   H_NumericEdit.edit.SetFixedWidth( hgEditWidth );
   G_NumericEdit.edit.SetFixedWidth( hgEditWidth );
   B_V_NumericEdit.edit.SetFixedWidth( hgEditWidth );
   D_NumericEdit.edit.SetFixedWidth( hgEditWidth );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF EphemerisGeneratorInterface.cpp - Released 2021-04-09T19:41:48Z
