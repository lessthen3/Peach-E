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

#include "../PeachNode.h"

namespace PeachCore {

    class PeachNode2D : public PeachNode 
    {
    public:
        virtual ~PeachNode2D();

        //PeachNode2D(){}
        PeachNode2D():PeachNode() {}

        PeachNode2D(const string& fp_Name) : PeachNode(fp_Name) {};
        
        virtual void 
            OnEnter() = 0;

        virtual void 
            OnUpdate(float fp_TimeSinceLastFrame) = 0;

        virtual void 
            OnConstantUpdate(float fp_TimeSinceLastFrame) = 0;

        virtual void 
            OnExit() = 0;

        virtual void 
            QueueRemoval() = 0; //queues for removal from scene tree at end of frame or whenever is convenient idk
    };
}