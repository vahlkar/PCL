#ifndef __StarNetProcess_h
#define __StarNetProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class StarNetProcess : public MetaProcess
{
public:

   StarNetProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   uint32 Version() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;

   void SetRGBWeightsFilePath( const String& );
   const String& RGBWeightsFilePath();

   void SetGrayscaleWeightsFilePath( const String& );
   const String& GrayscaleWeightsFilePath();
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern StarNetProcess* TheStarNetProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StarNetProcess_h
