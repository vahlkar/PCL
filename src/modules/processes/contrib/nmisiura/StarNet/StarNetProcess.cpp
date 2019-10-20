#include "StarNetInstance.h"
#include "StarNetInterface.h"
#include "StarNetParameters.h"
#include "StarNetProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl {
    #include "StarNetIcon.xpm"
    
    StarNetProcess* TheStarNetProcess = nullptr;
    StarNetProcess::StarNetProcess() : MetaProcess() {
        TheStarNetProcess = this;
       
        new StarNetStride(this);
        new StarNetMask(this);
    }

    IsoString StarNetProcess::Id() const {
        return "StarNet";
    }

    IsoString StarNetProcess::Category() const {
        return IsoString(); // No category
    }

    uint32 StarNetProcess::Version() const {
        return 0x100; // required
    }

    String StarNetProcess::Description() const {
        return
        "<html>"
        "<p>A newral network removing stars from stretched images.</p>"
        "</html>";
    }

    // ----------------------------------------------------------------------------

    const char** StarNetProcess::IconImageXPM() const {
        return StarNetIcon_XPM;
    }
    // ----------------------------------------------------------------------------

    ProcessInterface* StarNetProcess::DefaultInterface() const {
        return TheStarNetInterface;
    }
    // ----------------------------------------------------------------------------

    ProcessImplementation* StarNetProcess::Create() const {
        return new StarNetInstance(this);
    }

    // ----------------------------------------------------------------------------

    ProcessImplementation* StarNetProcess::Clone(const ProcessImplementation& p) const {
        const StarNetInstance* instance = dynamic_cast<const StarNetInstance*>(&p);
        return (instance != nullptr) ? new StarNetInstance(*instance) : nullptr;
    }

    // ----------------------------------------------------------------------------

    bool StarNetProcess::CanProcessCommandLines() const {
        return true;
    }

    // ----------------------------------------------------------------------------

    static void ShowHelp() {
       Console().Write(
        "<raw>"
        "Usage: StarNet [<arg_list>] [<view_list>]"
        "\n"
        "\n--interface"
        "\n"
        "\n      Launches the interface of this process."
        "\n"
        "\n--help"
        "\n"
        "\n      Displays this help and exits."
        "</raw>" );
    }

    int StarNetProcess::ProcessCommandLine(const StringList& argv) const {
        ArgumentList arguments = ExtractArguments(argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards);
        
        StarNetInstance instance(this);
        
        bool launchInterface = false;
        int count = 0;
        
        for (const Argument& arg : arguments) {
            if (arg.IsNumeric()) {
                throw Error("Unknown numeric argument: " + arg.Token());
            } else if (arg.IsString()) {
                throw Error("Unknown string argument: " + arg.Token());
            } else if (arg.IsSwitch()) {
                throw Error("Unknown switch argument: " + arg.Token());
            } else if (arg.IsLiteral()) {
                // These are standard parameters that all processes should provide.
                if (arg.Id() == "-interface") launchInterface = true;
                else if (arg.Id() == "-help") {
                    ShowHelp();
                    return 0;
                } else throw Error("Unknown argument: " + arg.Token());
            } else if (arg.IsItemList()) {
                ++count;
                if (arg.Items().IsEmpty()) throw Error("No view(s) found: " + arg.Token());
                
                for (StringList::const_iterator j = arg.Items().Begin(); j != arg.Items().End(); ++j) {
                    View v = View::ViewById(*j);
                    if (v.IsNull()) throw Error("No such view: " + *j);
                    instance.LaunchOn(v);
                }
            }
        }
        
        if (launchInterface)
            instance.LaunchInterface();
        else if (count == 0) {
          if (ImageWindow::ActiveWindow().IsNull())
             throw Error("There is no active image window.");
             instance.LaunchOnCurrentView();
        }
        return 0;
    }
}