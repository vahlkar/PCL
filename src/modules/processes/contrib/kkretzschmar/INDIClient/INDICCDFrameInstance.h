//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.7
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.1
// ----------------------------------------------------------------------------
// INDICCDFrameInstance.h - Released 2020-12-17T15:46:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2020 Klaus Kretzschmar
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

#ifndef __INDICCDFrameInstance_h
#define __INDICCDFrameInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class INDICCDFrameInstance : public ProcessImplementation
{
public:

   INDICCDFrameInstance( const MetaProcess* );
   INDICCDFrameInstance( const INDICCDFrameInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter*, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter*, size_type tableRow ) const override;

   bool ValidateDevice( bool throwErrors = true ) const;
   String TelescopeDeviceName( bool throwErrors = true ) const;

   void SendDeviceProperties( bool asynchronous = true ) const;

   String FileNameFromTemplate( const String& fileNameTemplate ) const;
   String ServerFileName() const;
   String ClientFileName() const;

   static String UploadModePropertyString( int uploadModeIdx );
   static String CCDFrameTypePropertyString( int frameTypeIdx );
   static String CCDFrameTypePrefix( int frameTypeIdx );

   pcl_enum GetPerSide( const String& telescopeName, double currentLST, double currentRA );

private:

   String p_deviceName;
   pcl_enum p_uploadMode;
   String p_serverUploadDirectory;
   String p_serverFileNameTemplate;
   pcl_enum p_frameType;
   int32 p_binningX;
   int32 p_binningY;
   int32 p_filterSlot;
   double p_exposureTime;
   double p_exposureDelay;
   int32 p_exposureCount;
   pcl_bool p_openClientImages;
   String p_newImageIdTemplate;
   pcl_bool p_reuseImageWindow;
   pcl_bool p_autoStretch;
   pcl_bool p_linkedAutoStretch;
   pcl_bool p_saveClientImages;
   pcl_bool p_overwriteClientImages;
   String p_clientDownloadDirectory;
   String p_clientFileNameTemplate;
   String p_clientOutputFormatHints;
   String p_objectName;
   pcl_enum p_telescopeSelection;
   pcl_bool p_requireSelectedTelescope;
   String p_telescopeDeviceName; // only if p_telescopeSelection = device name
   String p_extFilterWheelDeviceName;
   pcl_bool p_enableAlignmentCorrection;
   String p_alignmentFile;
   int32 p_telescopeFocalLength;
   pcl_bool p_applyPlateSolver;
   pcl_bool p_centerTarget;
   String   p_serverURL;
   String   p_solverCatalogName;
   pcl_bool p_solverAutoCatalog;
   int32    p_solverLimitMagnitude;
   pcl_bool p_solverAutoLimitMagnitude;
   float    p_solverStarSensitivity;
   int32    p_solverNoiseLayers;
   pcl_enum p_solverAlignmentDevice;
   pcl_bool p_solverDistortionCorrection;
   float    p_solverSplineSmoothing;
   pcl_enum p_solverProjection;


   StringList o_clientViewIds;
   StringList o_clientFilePaths;
   StringList o_serverFrames;

   int m_exposureNumber = 0;

   void SetTelescopeAlignmentModelParameter( bool throwErrors = false );
   void SetTelescopeFocalLength();

   friend class INDICCDFrameInterface;
   friend class AbstractINDICCDFrameExecution;
};

// ----------------------------------------------------------------------------

class AbstractINDICCDFrameExecution
{
public:

   AbstractINDICCDFrameExecution( INDICCDFrameInstance& instance )
      : m_instance( instance )
   {
   }

   virtual ~AbstractINDICCDFrameExecution()
   {
   }

   virtual void Perform();
   virtual void Abort();

   const INDICCDFrameInstance& Instance() const
   {
      return m_instance;
   }

   bool IsRunning() const
   {
      return m_running;
   }

   bool WasAborted() const
   {
      return m_aborted;
   }

   int SuccessCount() const
   {
      return m_successCount;
   }

   int ErrorCount() const
   {
      return m_errorCount;
   }

protected:

   INDICCDFrameInstance& m_instance;

   virtual void StartAcquisitionEvent() = 0;

   virtual void NewExposureEvent( int expNum, int expCount ) = 0;

   virtual void StartExposureDelayEvent( double totalDelayTime ) = 0;
   virtual void ExposureDelayEvent( double expTime ) = 0;
   virtual void EndExposureDelayEvent() = 0;

   virtual void StartExposureEvent( int expNum, int expCount, double expTime ) = 0;
   virtual void ExposureEvent( int expNum, int expCount, double expTime ) = 0;
   virtual void ExposureErrorEvent( const String& errMsg ) = 0;
   virtual void EndExposureEvent( int expNum ) = 0;

   virtual void WaitingForServerEvent() = 0;

   virtual void NewFrameEvent( ImageWindow&, bool reusedWindow, bool geometryChanged ) = 0;
   virtual void NewFrameEvent( const String& filePath ) = 0;

   virtual void EndAcquisitionEvent() = 0;

   virtual void AbortEvent() = 0;

private:

   bool m_running = false;
   bool m_aborted = false;
   int m_successCount = 0;
   int m_errorCount = 0;

   // Geometry of the current image window
   static int s_width;
   static int s_height;
   static int s_numberOfChannels;

};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __INDICCDFrameInstance_h

// ----------------------------------------------------------------------------
// EOF INDICCDFrameInstance.h - Released 2020-12-17T15:46:56Z
