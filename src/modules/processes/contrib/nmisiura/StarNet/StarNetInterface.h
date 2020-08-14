#ifndef __StarNetInterface_h
#define __StarNetInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ToolButton.h>

#include "StarNetInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class StarNetInterface : public ProcessInterface
{
public:

   StarNetInterface();
   virtual ~StarNetInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   StarNetInstance m_instance;

   struct GUIData
   {
      GUIData( StarNetInterface& );

      VerticalSizer     Global_Sizer;
         HorizontalSizer   StrideParameter_Sizer;
            Label             StrideParameter_Label;
            ComboBox          StrideParameter_ComboBox;
         HorizontalSizer   MaskParameter_Sizer;
            CheckBox          MaskParameter_CheckBox;
         HorizontalSizer   RGBWeightsFile_Sizer;
            Label             RGBWeightsFile_Label;
            Edit              RGBWeightsFile_Edit;
            ToolButton        RGBWeightsFile_ToolButton;
         HorizontalSizer   GrayscaleWeightsFile_Sizer;
            Label             GrayscaleWeightsFile_Label;
            Edit              GrayscaleWeightsFile_Edit;
            ToolButton        GrayscaleWeightsFile_ToolButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __ItemClicked( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __EditCompleted( Edit& sender );
   void __Click( Button& sender, bool checked );
   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   friend struct GUIData;
};

PCL_BEGIN_LOCAL
extern StarNetInterface* TheStarNetInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StarNetInterface_h
