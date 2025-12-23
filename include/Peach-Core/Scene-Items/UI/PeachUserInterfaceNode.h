/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:         
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

///PeachCore
#include "../../Utils/Logger.h"

///font stuff
#include <msdfgen/msdfgen.h>
#include <msdfgen/msdfgen-ext.h>

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace PeachCore{
namespace PUI{

struct TextStyle 
{
    float CurrentFontSize = 0.0f;
    string TypefaceName;
    bool IsMonoSpaced = true;
};

enum class NodeType
{
    None,
    Root, 
    Window,
    Button, 
    Label, 
    Image, 
    Panel, 
    ScrollArea, 
    MenuBar,
    MenuItem, 
    Popup, 
    Slider
};

struct PeachUserInterfaceNode 
{
    NodeType m_Type = NodeType::None; //default to no type, i like default constructors, they're cool

    bool m_Dirty = true; // needs redraw/layout dirty little kitten >w<

    uint64_t pm_NodeID; //used to identify node uniquely, is a string so that users can just type: "node.remove("myNodeName")"
    string pm_PeachName;

    unordered_map<uint64_t, PeachUserInterfaceNode> pm_Children; //have to use pointers since C++ slices and dices my types >w<

    PeachUserInterfaceNode* m_Parent = nullptr; //not a unique ptr since parents should not be owned by their children

    // -- Creation --
    PeachUserInterfaceNode() = default;

    virtual ~PeachUserInterfaceNode() = default;

    virtual PeachUserInterfaceNode*
        AddChild(unique_ptr<PeachUserInterfaceNode>&& fp_Child)
    {
        fp_Child->m_Parent = this;
        //pm_Children.insert(move(fp_Child));
        //m_Dirty = true;
        //return m_Children.back().get();
    }

    virtual void
        RemoveChild(const string& fp_DesiredNode)
    {
        //Do a DFS search here to find the node to remove
    }
};

}//namespace PUI
}//namespace PeachCore
