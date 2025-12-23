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

/*
This Class contains all algorithms that run and determine state between all PUI nodes uwu, including walking the tree to determine hit detection, what elements are 
*/

#include <glm/glm.hpp>

///STL
#include "../Scene-Items/UI/Panel.h"

namespace PeachCore{
namespace PUI{

    struct Section //defines a container that should evenly divide the screen into pieces where AABB checks can easily be done to determine whether a bounding box is contained inside
    {
        glm::vec2 TopLeftCorner = glm::vec2();
        glm::vec2 BottomRightCorner = glm::vec2();

        vector<uint64_t> ContainedNodes;

    };

    struct UserInterfaceManager
    {
        // public:
        //     unique_ptr<PeachUserInterfaceNode> root;
        //     shared_ptr<Logger> pui_logger = nullptr;

        //     PeachUIManager() 
        //     {
        //         root = make_unique<PeachUserInterfaceNode>(NodeType::Root);
        //         root->m_Rectangle = {0, 0, 1920, 1080}; // Example
        //     }

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
            // PeachUserInterfaceNode* HitTest(float x, float y)
            // {
            //     // Walk tree, return node under point (for mouse events)
            //     return nullptr;
            // }
    };
}//namespace PUI
}//namespace PeachCore