#include "StarNetInterface.h"
#include "StarNetParameters.h"
#include "StarNetProcess.h"

namespace pcl {
    StarNetInterface* TheStarNetInterface = nullptr;

    #include "StarNetIcon.xpm"

    StarNetInterface::StarNetInterface():instance(TheStarNetProcess) {
       TheStarNetInterface = this;
    }

    StarNetInterface::~StarNetInterface() {
       if (GUI != nullptr) delete GUI, GUI = nullptr;
    }

    IsoString StarNetInterface::Id() const {
       return "StarNet";
    }

    MetaProcess* StarNetInterface::Process() const {
       return TheStarNetProcess;
    }

    const char** StarNetInterface::IconImageXPM() const {
       return StarNetIcon_XPM;
    }

    void StarNetInterface::ApplyInstance() const {
       instance.LaunchOnCurrentView();
    }

    void StarNetInterface::ResetInstance() {
       StarNetInstance defaultInstance(TheStarNetProcess);
       ImportProcess(defaultInstance);
    }

    bool StarNetInterface::Launch(const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/) {
        if (GUI == nullptr) {
            GUI = new GUIData(*this);
            SetWindowTitle("StarNet++ (beta)");
            UpdateControls();
       }
       
       dynamic = false;
       return &P == TheStarNetProcess;
    }
    
    ProcessImplementation* StarNetInterface::NewProcess() const {
       return new StarNetInstance(instance);
    }
    
    bool StarNetInterface::ValidateProcess(const ProcessImplementation& p, String& whyNot) const {
        if (dynamic_cast<const StarNetInstance*>(&p) != nullptr) return true;
        whyNot = "Not a StarNet instance.";
        return false;
    }

    bool StarNetInterface::RequiresInstanceValidation() const {
        return true;
    }

    bool StarNetInterface::ImportProcess(const ProcessImplementation& p) {
        instance.Assign(p);
        UpdateControls();
        return true;
    }
    
    void StarNetInterface::UpdateControls() {
        GUI->MaskParameter_CheckBox.SetChecked(instance.p_mask);
        GUI->StrideParameter_ComboBox.SetCurrentItem(instance.p_stride);
    }

    void StarNetInterface::__ItemClicked(Button& sender, bool checked) {
       if (sender == GUI->MaskParameter_CheckBox) instance.p_mask = checked;
    }

    void StarNetInterface::__ItemSelected(ComboBox& sender, int itemIndex) {
       if (sender == GUI->StrideParameter_ComboBox) instance.p_stride = itemIndex;
    }

    // ----------------------------------------------------------------------------

    StarNetInterface::GUIData::GUIData(StarNetInterface& w)
    {
        pcl::Font font = w.Font();
        int labelWidth1 = font.Width(String("Replace orig:")); // the longest label text
        int editWidth1 = font.Width(String('0', 7));
       
        //

        StrideParameter_Label.SetText("Stride:");
        StrideParameter_Label.SetMinWidth(labelWidth1);
        StrideParameter_Label.SetToolTip("<p>Just leave it as it is.<br/>Image is transformed tile by tile, each tile having size STRIDE by STRIDE.\
                                             Decreasing this value might help with edge artifacts on large stars, but it is usually not\
                                             worth it.</p>");
        StrideParameter_Label.SetTextAlignment(TextAlign::Right|TextAlign::VertCenter);
        
        StrideParameter_ComboBox.AddItem("128");
        StrideParameter_ComboBox.AddItem("64");
        StrideParameter_ComboBox.AddItem("32");
        StrideParameter_ComboBox.AddItem("16");
        StrideParameter_ComboBox.AddItem("8");
        // StrideParameter_ComboBox.SetToolTip("<p>Too much to explain. Just leave it as it is.</p>");
        StrideParameter_ComboBox.OnItemSelected((ComboBox::item_event_handler)&StarNetInterface::__ItemSelected, w);
        
        StrideParameter_Sizer.SetSpacing(4);
        StrideParameter_Sizer.Add(StrideParameter_Label);
        StrideParameter_Sizer.Add(StrideParameter_ComboBox);
        StrideParameter_Sizer.AddStretch();
        
        //
        
        MaskParameter_CheckBox.SetText("Create starmask");
        MaskParameter_CheckBox.SetToolTip("<p>Generate star mask instead of starless image.</p>");
        MaskParameter_CheckBox.OnClick((pcl::Button::click_event_handler)&StarNetInterface::__ItemClicked, w);
        
        MaskParameter_Sizer.AddUnscaledSpacing(labelWidth1 + w.LogicalPixelsToPhysical(4));
        MaskParameter_Sizer.Add(MaskParameter_CheckBox);
        MaskParameter_Sizer.AddStretch();
       
       //

        Global_Sizer.SetMargin(8);
        Global_Sizer.SetSpacing(6);
        Global_Sizer.Add(StrideParameter_Sizer);
        Global_Sizer.Add(MaskParameter_Sizer);

        w.SetSizer(Global_Sizer);
        w.AdjustToContents();
        w.SetFixedSize();
    }
}