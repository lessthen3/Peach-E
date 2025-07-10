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

#include "PeachNode2D.h"

#include <vector>
#include <string>

namespace PeachCore {

    struct Frame {
        //sf::IntRect rect;
        float duration;
    };

    struct Animation 
    {

    };

    class AnimationPlayer2D: public PeachNode
    {
        std::vector<Frame> frames;
        float m_TotalLength;
        float m_Progress;
        //sf::Sprite& m_Target;

    public:
        AnimationPlayer2D();
        ~AnimationPlayer2D();
        void AddFrame(Frame&& fp_Frame);
        void StepFrameForward(float fp_TimeSinceLastFrame);
        const float GetLength() const { return m_TotalLength; }


        void Initialize();
        void Update();
        void ConstantUpdate();
        void QueueFree();
        void OnSceneTreeExit();

        // nlohmann::json SerializePeachNodeToJSON();

    };

}