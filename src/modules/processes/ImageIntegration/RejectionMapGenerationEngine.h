//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.4.9
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 1.2.33
// ----------------------------------------------------------------------------
// RejectionMapGenerationEngine.h - Released 2021-04-09T19:41:48Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#ifndef __RejectionMapGenerationEngine_h
#define __RejectionMapGenerationEngine_h

#include "ImageIntegrationInstance.h"
#include "IntegrationEngineBase.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class RejectionMapGenerationEngine : public IntegrationEngineBase
{
public:

   RejectionMapGenerationEngine( const ImageIntegrationInstance&, StatusMonitor&,
                                 Image& map, bool high, int channel );

   virtual ~RejectionMapGenerationEngine();

   void Generate();

private:

   class GenerationThread : public EngineThreadBase
   {
   public:

      GenerationThread( RejectionMapGenerationEngine& engine, int firstRow, int endRow )
         : EngineThreadBase( engine, firstRow, endRow )
         , m_engine( engine )
      {
      }

      void Run() override;

   private:

      RejectionMapGenerationEngine& m_engine;
   };

   typedef ReferenceArray<GenerationThread> thread_list;

   Image&      m_map;
   bool        m_high;
   int         m_channel;
   thread_list m_threads;

   friend class GenerationThread;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RejectionMapGenerationEngine_h

// ----------------------------------------------------------------------------
// EOF RejectionMapGenerationEngine.h - Released 2021-04-09T19:41:48Z
