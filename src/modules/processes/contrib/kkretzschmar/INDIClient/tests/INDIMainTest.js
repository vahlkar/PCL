// ----------------------------------------------------------------------------
// PixInsight JavaScript Runtime API - PJSR Version 1.0
// ----------------------------------------------------------------------------
// INDIMainTest.js - Released 2024-12-28T16:54:15Z
// ----------------------------------------------------------------------------
//
//
// Copyright (c) 2014-2024 Klaus Kretzschmar
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

#include <pjsr/Test.jsh>

#include "INDICCDControllerTests.js"

#define host "localhost"

function INDICCDTestSuite()
{
   this.__base__ = Test;
   this.__base__( "INDICCDTestSuite" );

   this.deviceController = new IndigoDeviceController;
   this.deviceController.serverHostName = "localhost"
   this.timer = new ElapsedTime;

   this.add( new INDICCDControllerTests( this ) );

   this.executeController = function()
   {
      if ( !this.deviceController.executeGlobal() )
         throw new Error( "IndigoDeviceController.executeGlobal() failed" );
   };

   this.restartTimer = function()
   {
      this.timer.reset();
   };

   this.stopTimer = function ()
   {
      this.timer.stop();
   };
   this.timeout = function()
   {
      if ( this.timer.value > 10 )
      {
         console.criticalln( "Timeout reached" );
         return true;
      }
      return false;
   };

   // N.B. begin() and end() methods reimplemented from Test

   this.begin = function()
   {
      // Connect to INDI server
      this.deviceController.serverHostName = host
      this.deviceController.serverConnect = true;
      this.executeController();

      // Wait until device names are received from server
      for ( this.restartTimer(); !this.timeout(); )
      {
         msleep( 100 );
         processEvents();
         if ( indexOfDevice( (new IndigoDeviceController).devices, CCD_DEVICE_NAME ) >= 0 )
            break;
      }
      this.stopTime();

      // Wait for another 500 ms to allow for all device and property lists to
      // update completely.
      msleep( 500 );

      // Connect to CCD device
      let propertyKey = "/" + CCD_DEVICE_NAME + "/CONNECTION/CONNECTED";
      this.deviceController.newProperties = [[propertyKey, "INDI_SWITCH", "ON"]];
      this.deviceController.serverCommand = "SET";
      this.executeController();
      this.deviceController.serverCommand = "";

      // Wait until device is connected
      for ( this.restartTimer(); !this.timeout(); )
      {
         msleep( 100 );
         processEvents();
         if ( propertyEquals( (new IndigoDeviceController).properties, propertyKey, "ON" ) )
            break;
      }
   };

   this.end = function()
   {
      // Disconnect from INDI server
      this.deviceController.serverConnect = false;
      this.executeController();
   };
};

#include "INDIMountControllerTests.js"

function INDIMountTestSuite()
{
   this.__base__ = Test;
   this.__base__( "INDIMountTestSuite" );

<<<<<<< HEAD
   this.deviceController = new IndigoDeviceController;
=======
   this.deviceController = new INDIDeviceController;
   this.deviceController.serverHostName = "klaus-Inspiron-1720"
>>>>>>> 7c912548af71aec64089d296987ff43fbbe876ba
   this.timer = new ElapsedTime;

   this.add( new INDIMountControllerTests( this ) );

   this.executeController = function()
   {
      if ( !this.deviceController.executeGlobal() )
         throw new Error( "IndigoDeviceController.executeGlobal() failed" );
   };

   this.restartTimer = function()
   {
      this.timer.reset();
   };

   this.timeout = function()
   {
      if ( this.timer.value > 10 )
      {
         console.criticalln( "Timeout reached" );
         return true;
      }
      return false;
   };

   // N.B. begin() and end() methods reimplemented from Test

   this.begin = function()
   {
      // Connect to INDI server
      this.deviceController.serverConnect = true;
      this.executeController();

      // Wait until device names are received from server
      for ( this.restartTimer(); !this.timeout(); )
      {
         msleep( 100 );
         processEvents();
         if ( indexOfDevice( (new IndigoDeviceController).devices, MOUNT_DEVICE_NAME ) >= 0 )
            break;
      }

      // Wait for another 500 ms to allow for all device and property lists to
      // update completely.
      msleep( 500 );

      // Connect to Mount device
      let propertyKey = "/" + MOUNT_DEVICE_NAME + "/CONNECTION/CONNECTED";
      this.deviceController.newProperties = [[propertyKey, "INDI_SWITCH", "ON"]];
      this.deviceController.serverCommand = "SET";
      this.executeController();
      this.deviceController.serverCommand = "";

      // Wait until device is connected
      for ( this.restartTimer(); !this.timeout(); )
      {
         msleep( 100 );
         processEvents();
         if ( propertyEquals( (new IndigoDeviceController).properties, propertyKey, "ON" ) )
            break;
      }
   };

   this.end = function()
   {
      // Disconnect from INDI server
      this.deviceController.serverConnect = false;
      this.executeController();
   };
};

// run CCD tests
//INDICCDTestSuite.prototype = new Test;

//(new INDICCDTestSuite).run();

// run Mount tests
INDIMountTestSuite.prototype = new Test;

(new INDIMountTestSuite).run();

// ----------------------------------------------------------------------------
// EOF INDIMainTest.js - Released 2024-12-28T16:54:15Z
