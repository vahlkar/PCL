//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.8.5
// ----------------------------------------------------------------------------
// pcl/ButtonCodes.h - Released 2024-12-28T16:53:48Z
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

#ifndef __PCL_ButtonCodes_h
#define __PCL_ButtonCodes_h

/// \file pcl/ButtonCodes.h

#include <pcl/Defs.h>

#include <pcl/Flags.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::MouseButton
 * \brief Defines PCL mouse button codes
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>MouseButton::Left</td>    <td>Primary mouse button</td></tr>
 * <tr><td>MouseButton::Right</td>   <td>Secondary mouse button</td></tr>
 * <tr><td>MouseButton::Middle</td>  <td>Middle mouse button</td></tr>
 * <tr><td>MouseButton::X1</td>      <td>First X button</td></tr>
 * <tr><td>MouseButton::X2</td>      <td>Second X button</td></tr>
 * <tr><td>MouseButton::Unknown</td> <td>Unknown/unsupported mouse button</td></tr>
 * </table>
 */
namespace MouseButton
{
   enum mask_type
   {
      Unknown  = 0,     // Unknown/unsupported mouse button
      Left     = 0x01,  // Primary mouse button
      Right    = 0x02,  // Secondary mouse button
      Middle   = 0x04,  // Middle mouse button
      X1       = 0x10,  // First X button
      X2       = 0x20   // Second X button
   };
}

/*!
 * A combination of mouse button codes.
 */
using MouseButtons = Flags<MouseButton::mask_type>;

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::KeyModifier
 * \brief Defines PCL key modifier codes
 *
 * The %KeyModifier namespace defines platform-independent key modifiers on the
 * PixInsight/PCL framework. Key modifiers are sent to event handlers (such as
 * Control::OnKeyPress(), Control::OnMousePress(), etc.) and used to specify
 * keyboard shortcuts; see for example Action::SetAccelerator().
 *
 * To query the current state of supported keyboard modifier keys in real time,
 * see the pcl::KeyboardModifier namespace and the CurrentKeyboardModifiers()
 * global function, defined and declared, respectively, in the pcl/KeyCodes.h
 * header file.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>KeyModifier::Shift</td>    <td>Shift key</td></tr>
 * <tr><td>KeyModifier::Control</td>  <td>Control key (Command key on macOS)</td></tr>
 * <tr><td>KeyModifier::Alt</td>      <td>Alt key</td></tr>
 * <tr><td>KeyModifier::SpaceBar</td> <td>Space bar</td></tr>
 * <tr><td>KeyModifier::Meta</td>     <td>Meta key (Control key on macOS)</td></tr>
 * </table>
 */
namespace KeyModifier
{
   enum mask_type
   {
      Shift    = 0x01,  // Shift key
      Control  = 0x02,  // Control key (= Command on macOS)
      Alt      = 0x04,  // Alt key
      SpaceBar = 0x08,  // Space bar
      Meta     = 0x10   // Meta key (= Control on macOS)
   };
}

/*!
 * A combination of keyboard modifiers.
 */
using KeyModifiers = Flags<KeyModifier::mask_type>;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ButtonCodes_h

// ----------------------------------------------------------------------------
// EOF pcl/ButtonCodes.h - Released 2024-12-28T16:53:48Z
