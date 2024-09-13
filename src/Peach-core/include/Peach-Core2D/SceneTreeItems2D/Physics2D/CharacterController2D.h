#pragma once

<<<<<<< Updated upstream:src/Peach-core/include/Peach-Core2D/SceneTreeItems2D/Physics2D/CharacterController2D.h
#include "../../../Managers/Physics2DManager.h"
=======
#include "../Managers/Physics2DManager.h"
>>>>>>> Stashed changes:include/Peach-Core/2D/CharacterController2D.h
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