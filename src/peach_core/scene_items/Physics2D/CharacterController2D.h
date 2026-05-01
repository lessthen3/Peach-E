/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

#include "managers/PhysicsManager.h"

#include <glm/glm.hpp>

namespace PeachCore {

    class CharacterController2D 
    {
        CharacterController2D();
        ~CharacterController2D();


        void MoveAndSlide(const glm::vec2& fp_Velocity)
        {
            //b2Vec2 currentVelocity = pm_CharacterBody2D->GetLinearVelocity();
            // Set the desired velocity
            /*currentVelocity.x = fp_Velocity.x;
            currentVelocity.y = fp_Velocity.y;*/
            //pm_CharacterBody2D->SetLinearVelocity(currentVelocity);

            // Further adjustments are made post-collision in the contact listener
        }


    private:
        b2BodyId pm_CharacterBody2D = b2_nullBodyId;
    };

}