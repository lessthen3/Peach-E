#pragma once

#include "../General/PeachNode.h"

namespace PeachCore {

	class PeachNode2D : public PeachNode 
	{
	public:
		virtual ~PeachNode2D();

		//PeachNode2D(){}
		PeachNode2D():PeachNode() {}

		PeachNode2D(const string& fp_Name) : PeachNode(fp_Name) {};
		
		virtual void 
			Initialize() = 0;

		virtual void 
			Update(float fp_TimeSinceLastFrame) = 0;

		virtual void 
			ConstantUpdate(float fp_TimeSinceLastFrame) = 0;

		virtual void 
			OnSceneTreeExit() = 0;

		virtual void 
			QueueRemoval() = 0; //queues for removal from scene tree at end of frame or whenever is convenient idk

		virtual void 
			Draw() = 0;

		virtual nlohmann::json 
			SerializePeachNodeToJSON() = 0;
	};
}