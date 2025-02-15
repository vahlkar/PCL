//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.2
// ----------------------------------------------------------------------------
// FilterManagerParameters.cpp - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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

#include "FilterManagerParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FMTask*          TheFMTaskParameter = nullptr;
FMPath*          TheFMPathParameter = nullptr;
FMNoGUIMessages* TheFMNoGUIMessagesParameter = nullptr;

// ----------------------------------------------------------------------------

FMTask::FMTask( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheFMTaskParameter = this;
}

IsoString FMTask::Id() const
{
   return "task";
}

size_type FMTask::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString FMTask::ElementId( size_type index ) const
{
   switch ( index )
   {
   default:
   case NoAction:     return "Task_NoAction";
   case ImportXML:    return "Task_ImportXML";
   case MergeXML:     return "Task_MergeXML";
   case ImportCSV:    return "Task_ImportCSV";
   case MergeCSV:     return "Task_MergeCSV";
   case ExportXML:    return "Task_ExportXML";
   case ExportCSV:    return "Task_ExportCSV";
   case MergeDefault: return "Task_MergeDefault";
   case SetDefault:   return "Task_SetDefault";
   }
}

int FMTask::ElementValue( size_type index ) const
{
   return int( index );
}

size_type FMTask::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

FMPath::FMPath( MetaProcess* P )
   : MetaString( P )
{
   TheFMPathParameter = this;
}

IsoString FMPath::Id() const
{
   return "path";
}

// ----------------------------------------------------------------------------

FMNoGUIMessages::FMNoGUIMessages( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFMNoGUIMessagesParameter = this;
}

IsoString FMNoGUIMessages::Id() const
{
   return "noGUIMessages";
}

bool FMNoGUIMessages::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FilterManagerParameters.cpp - Released 2024-12-28T16:54:15Z
