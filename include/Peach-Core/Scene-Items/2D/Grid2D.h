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

	class Grid2D: public PeachNode
	{
	public:
		virtual void Initialize();
		virtual void Update(float fp_TimeSinceLastFrame);
		virtual void ConstantUpdate(float fp_TimeSinceLastFrame);
		virtual void OnSceneTreeExit();
		virtual void QueueRemoval();

		virtual void //nlohmann::json 
			SerializePeachNodeToJSON();

		Grid2D() = default;
		~Grid2D() = default;
	};

}