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
#include "../../Managers/LogManager.h"

///Vulkan
#include <volk.h>

///SDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>
#include <SDL3/SDL_vulkan.h>

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
    float Size = 0.0f;
    float Radius = 0.0f;
    // Add more as needed: font, textColor, shadow, etc.
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

struct VulkanShaderAssets
{
    VkPipeline Pipeline;
    VkPipelineLayout Layout;
    VkShaderModule VertShader;
    VkShaderModule FragShader;

    // Optionally: descriptorSetLayout, pipelineCache, etc.
    string Name; // For debugging/user selection
};

struct OpenGLShaderAssets
{

};


class PeachUserInterfaceNode 
{
public:
    NodeType m_Type = NodeType::None;


    vector<unique_ptr<PeachUserInterfaceNode>> m_Children;

    PeachUserInterfaceNode* m_Parent = nullptr; //not a unique ptr since parents should not be owned by their children just like irl lmfao

    bool m_Dirty = true; // needs redraw/layout dirty little kitten >w<

    // Optional: text, image id, callbacks, etc.
    string text;
    int textureId = -1;

    // -- Creation --
    PeachUserInterfaceNode(NodeType fp_Type)
    {
        m_Type = fp_Type;
    }

    virtual ~PeachUserInterfaceNode() = default;

    virtual PeachUserInterfaceNode*
        AddChild(unique_ptr<PeachUserInterfaceNode> fp_Child)
    {
        fp_Child->m_Parent = this;
        m_Children.push_back(move(fp_Child));
        m_Dirty = true;
        return m_Children.back().get();
    }

    virtual void
        RemoveChild()
    {

    }
};

}//namespace PUI
}//namespace PeachCore
