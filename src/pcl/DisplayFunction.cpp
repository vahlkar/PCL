//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/DisplayFunction.cpp - Released 2024-12-28T16:53:56Z
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

#include <pcl/DisplayFunction.h>

namespace pcl
{

// ----------------------------------------------------------------------------

template <class P>
static void DoApply( GenericImage<P>& image, const DisplayFunction& F )
{
   int i = image.FirstSelectedChannel();
   if ( i < image.NumberOfNominalChannels() )
   {
      int j = Max( image.LastSelectedChannel(), image.NumberOfNominalChannels()-1 );

      image.Status().Initialize( "Display function transformation", (1+j-i)*image.NumberOfSelectedPixels() );
      image.Status().DisableInitialization();

      try
      {
         image.PushSelections();
         for ( ; i <= j; ++i )
         {
            image.SelectChannel( i );
            F[i] >> image;
         }
         image.PopSelections();
         image.Status().EnableInitialization();
      }
      catch( ... )
      {
         image.Status().EnableInitialization();
         throw;
      }
   }
}

void DisplayFunction::Apply( pcl::Image& image ) const
{
   DoApply( image, *this );
}

void DisplayFunction::Apply( pcl::DImage& image ) const
{
   DoApply( image, *this );
}

void DisplayFunction::Apply( pcl::UInt8Image& image ) const
{
   DoApply( image, *this );
}

void DisplayFunction::Apply( pcl::UInt16Image& image ) const
{
   DoApply( image, *this );
}

void DisplayFunction::Apply( pcl::UInt32Image& image ) const
{
   DoApply( image, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/DisplayFunction.cpp - Released 2024-12-28T16:53:56Z
