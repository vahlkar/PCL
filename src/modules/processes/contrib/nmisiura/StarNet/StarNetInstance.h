#ifndef __StarNetInstance_h
#define __StarNetInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class StarNetInstance : public ProcessImplementation
{
public:

   StarNetInstance( const MetaProcess* );
   StarNetInstance( const StarNetInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   pcl_enum p_stride;
   pcl_bool p_mask;

   friend class StarNetEngine;
   friend class StarNetProcess;
   friend class StarNetInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StarNetInstance_h
