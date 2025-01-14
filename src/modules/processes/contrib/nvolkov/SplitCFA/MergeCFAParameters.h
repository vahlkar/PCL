//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 1.0.6
// ----------------------------------------------------------------------------
// MergeCFAParameters.h - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
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

#ifndef __MergeCFAParameters_h
#define __MergeCFAParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{
// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class MergeCFASourceImage0 : public MetaString
{
public:

   MergeCFASourceImage0( MetaProcess* );
   IsoString Id() const override;
};

extern MergeCFASourceImage0* TheMergeCFASourceImage0Parameter;

// ----------------------------------------------------------------------------

class MergeCFASourceImage1 : public MetaString
{
public:

   MergeCFASourceImage1( MetaProcess* );
   IsoString Id() const override;
};

extern MergeCFASourceImage1* TheMergeCFASourceImage1Parameter;

// ----------------------------------------------------------------------------

class MergeCFASourceImage2 : public MetaString
{
public:

   MergeCFASourceImage2( MetaProcess* );
   IsoString Id() const override;
};

extern MergeCFASourceImage2* TheMergeCFASourceImage2Parameter;

// ----------------------------------------------------------------------------

class MergeCFASourceImage3 : public MetaString
{
public:

   MergeCFASourceImage3( MetaProcess* );
   IsoString Id() const override;
};

extern MergeCFASourceImage3* TheMergeCFASourceImage3Parameter;

// ----------------------------------------------------------------------------

class MergeCFAOutputViewId : public MetaString
{
public:

   MergeCFAOutputViewId( MetaProcess* );
   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern MergeCFAOutputViewId* TheMergeCFAOutputViewIdParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __MergeCFAParameters_h

// ----------------------------------------------------------------------------
// EOF MergeCFAParameters.h - Released 2024-12-28T16:54:15Z
