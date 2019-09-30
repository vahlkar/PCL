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

#include "StarNetInstance.h"

namespace pcl {
    class StarNetInterface : public ProcessInterface {
    public:
        StarNetInterface();
        virtual ~StarNetInterface();
        
        virtual IsoString Id() const;
        virtual MetaProcess* Process() const;
        virtual const char** IconImageXPM() const;
        
        virtual void ApplyInstance() const;
        virtual void ResetInstance();
        
        virtual bool Launch(const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/);
        
        virtual ProcessImplementation* NewProcess() const;
        
        virtual bool ValidateProcess(const ProcessImplementation&, String& whyNot) const;
        virtual bool RequiresInstanceValidation() const;
        
        virtual bool ImportProcess(const ProcessImplementation&);
        
    private:
        StarNetInstance instance;
        
        struct GUIData
        {
            GUIData( StarNetInterface& );
            
            VerticalSizer     Global_Sizer;
            HorizontalSizer   StrideParameter_Sizer;
            Label             StrideParameter_Label;
            ComboBox          StrideParameter_ComboBox;
            HorizontalSizer   MaskParameter_Sizer;
            CheckBox          MaskParameter_CheckBox;
        };
        
        GUIData* GUI = nullptr;
        
        void UpdateControls();
        
        void __RealValueUpdated(NumericEdit& sender, double value);
        void __IntegerValueUpdated(SpinBox& sender, int value);
        void __ItemClicked(Button& sender, bool checked);
        void __ItemSelected(ComboBox& sender, int itemIndex);
        void __EditGetFocus(Control& sender);
        void __EditCompleted(Edit& sender);
        
        friend struct GUIData;
    };
    
    PCL_BEGIN_LOCAL
    extern StarNetInterface* TheStarNetInterface;
    PCL_END_LOCAL
}
#endif