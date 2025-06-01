/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
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

///STB stuff
#include <stb/stb_truetype.h>

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

constexpr uint32_t MAX_TEXT_SIZE = 128;

namespace PeachCore{

    enum class InputType
    {
        KeyDown,
        KeyUp,
        MouseDown,
        MouseUp,
        MouseMove,
        MouseScroll,
        TextInput
    };

    struct KeyboardEvent 
    {
        InputType type;
        SDL_Scancode key;         // For key input
        
        char text[MAX_TEXT_SIZE]; // For typing
        uint64_t timestamp;
        bool consumed = false;    // UI can mark this so it doesn't reach gameplay
    };

    struct MouseEvent
    {
        SDL_Scancode key;         // For key input
        uint32_t mouseButton;     // SDL_BUTTON_LEFT, etc.
        float mouseX, mouseY;     // World-space or screen-space (decide!)
        float deltaX, deltaY;     // For motion
        int wheelX, wheelY;       // For scroll
    };

    struct GamePadEvent
    {

    };

    struct InputState 
    {
        unordered_map<SDL_Scancode, bool> IsDown;
        unordered_map<SDL_Scancode, uint64_t> LastPressedTimestamp;
        unordered_map<SDL_Scancode, int> RepeatCount;
        unordered_map<uint8_t, bool> MouseButtonDown;
        float MouseX = 0.0f;
        float MouseY = 0.0f;
        float MouseDeltaX = 0.0f;
        float MouseDeltaY = 0.0f;
        float ScrollX = 0.0f;
        float ScrollY = 0.0f;
    };


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

struct Rectangle //(x, y) dictates top left corner, width and height dictate how far the bottom right vert is extended
{
    float PosX  = 0.0f;
    float PosY = 0.0f;
    float Width = 0.0f; 
    float Height = 0.0f;

    inline bool
        IsWithinRectangle(const float fp_X, const float fp_Y)
        const noexcept
    {
        return
            (
                (abs(fp_X) > abs(PosX) and abs(fp_X) < (abs(PosX) + abs(Width)))
                and
                (abs(fp_Y) > abs(PosY) and abs(fp_Y) < (abs(PosY) + abs(Height)))
            );
    }
};



struct ShaderAssets
{
    VkPipeline Pipeline;
    VkPipelineLayout Layout;
    VkShaderModule VertShader;
    VkShaderModule FragShader;

    // Optionally: descriptorSetLayout, pipelineCache, etc.
    string Name; // For debugging/user selection
};


class PeachUserInterfaceNode 
{
public:
    NodeType m_Type = NodeType::None;

    Rectangle m_Rectangle;

    vector<unique_ptr<PeachUserInterfaceNode>> m_Children;

    PeachUserInterfaceNode* m_Parent = nullptr; //not a unique ptr since parents should not be owned by their children just like irl lmfao

    bool m_Dirty = true; // needs redraw/layout

    // Optional: text, image id, callbacks, etc.
    string text;
    int textureId = -1;

    // -- Creation --
    PeachUserInterfaceNode(NodeType fp_Type)
    {
        m_Type = fp_Type;
    }

    virtual ~PeachUserInterfaceNode() = default;

    //virtual void 
    //    DrawNode() = 0;

    PeachUserInterfaceNode*
        AddChild(unique_ptr<PeachUserInterfaceNode> fp_Child)
    {
        fp_Child->m_Parent = this;
        m_Children.push_back(move(fp_Child));
        m_Dirty = true;
        return m_Children.back().get();
    }

    void
        RemoveChild()
    {

    }
};

// -- Peach UI System --
class PeachUIManager
{
public:
    unique_ptr<PeachUserInterfaceNode> root;
    shared_ptr<LogManager> pui_logger = nullptr;

    PeachUIManager() 
    {
        root = make_unique<PeachUserInterfaceNode>(NodeType::Root);
        root->m_Rectangle = {0, 0, 1920, 1080}; // Example
    }

    // Walk tree, collect visible nodes, output draw data for batching
    // void collectDrawCommands(vector<YourDrawCommand>& outCmds) 
    // {
    //     collectDrawCommandsRecursive(root.get(), outCmds);
    // }

    // // Recursive collection (do layout/visibility/etc)
    // void collectDrawCommandsRecursive(PeachUINode* node, vector<YourDrawCommand>& outCmds)
    //  {
    //     // Build YourDrawCommand from node (rect, style, text, image, etc)
    //     // For each child:
    //     for (auto& child : node->children) {
    //         collectDrawCommandsRecursive(child.get(), outCmds);
    //     }
    // }

    // Hit-testing for input
    PeachUserInterfaceNode* HitTest(float x, float y)
    {
        // Walk tree, return node under point (for mouse events)
        return nullptr;
    }
};

}//namespace PUI
}//namespace PeachCore
