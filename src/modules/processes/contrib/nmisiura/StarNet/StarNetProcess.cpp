#include "StarNetProcess.h"
#include "StarNetInstance.h"
#include "StarNetInterface.h"
#include "StarNetParameters.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

StarNetProcess* TheStarNetProcess = nullptr;

static String s_rgbWeightsFilePath;
static String s_grayscaleWeightsFilePath;

// ----------------------------------------------------------------------------

StarNetProcess::StarNetProcess()
{
   TheStarNetProcess = this;

   new SNStride( this );
   new SNCreateMask( this );
}

// ----------------------------------------------------------------------------

IsoString StarNetProcess::Id() const
{
   return "StarNet";
}

// ----------------------------------------------------------------------------

IsoString StarNetProcess::Category() const
{
   return IsoString(); // No category
}

// ----------------------------------------------------------------------------

uint32 StarNetProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String StarNetProcess::Description() const
{
   return "<html>"
          "<p>A neural network removing stars from stretched images.</p>"
          "</html>";
}

// ----------------------------------------------------------------------------

String StarNetProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/StarNet.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* StarNetProcess::DefaultInterface() const
{
   return TheStarNetInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* StarNetProcess::Create() const
{
   return new StarNetInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* StarNetProcess::Clone( const ProcessImplementation& p ) const
{
   const StarNetInstance* instance = dynamic_cast<const StarNetInstance*>( &p );
   return (instance != nullptr) ? new StarNetInstance( *instance ) : nullptr;
}

// ----------------------------------------------------------------------------

void StarNetProcess::SetRGBWeightsFilePath( const String& filePath )
{
   if ( !File::Exists( filePath ) )
      throw Error( "The specified weights database file (RGB) does not exist: <raw>" + filePath + "</raw>" );
   s_rgbWeightsFilePath = filePath;
}

const String& StarNetProcess::RGBWeightsFilePath()
{
   return s_rgbWeightsFilePath;
}

void StarNetProcess::SetGrayscaleWeightsFilePath( const String& filePath )
{
   if ( !File::Exists( filePath ) )
      throw Error( "The specified weights database file (grayscale) does not exist: <raw>" + filePath + "</raw>" );
   s_grayscaleWeightsFilePath = filePath;
}

const String& StarNetProcess::GrayscaleWeightsFilePath()
{
   return s_grayscaleWeightsFilePath;
}

// ----------------------------------------------------------------------------

} // pcl
