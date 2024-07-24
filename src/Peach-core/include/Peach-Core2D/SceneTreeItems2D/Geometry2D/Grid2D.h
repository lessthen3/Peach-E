#pragma once

#include "../../../Unsorted/PeachNode.h"

namespace PeachCore {

	class Grid2D: public PeachNode
	{
	public:
		virtual void Initialize();
		virtual void Update(float fp_TimeSinceLastFrame);
		virtual void ConstantUpdate(float fp_TimeSinceLastFrame);
		virtual void OnSceneTreeExit();
		virtual void QueueRemoval();

		virtual nlohmann::json SerializePeachNodeToJSON();

		Grid2D() = default;
		~Grid2D() = default;
	};

}