#pragma once


#include <nlohmann/json.hpp>

#include <string>
#include <vector>
#include <memory>
#include "../Unsorted/PeachObject.h"

using namespace std;

namespace PeachCore {

	class PeachNode : public PeachObject
	{
	public:
		virtual ~PeachNode();

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

		virtual nlohmann::json 
			SerializePeachNodeToJSON() = 0;

		bool IsPausable = true;
		bool IsVisible = true;
		bool ShouldBeInTree = false;

		unsigned int m_DrawingLayer = 0;

		vector<unique_ptr<PeachNode>> m_Children;

		PeachNode& GetPeachNode(); //returns a reference to the desired PeachNode

		bool ReparentPeachNode(); //returns true if operation was successful, returns false otherwise

		void
			PrintTree();

		bool 
			IsInsideTree();

		bool 
			HasPeachNode();

		void 
			GetViewPort();

		string
			GetPathInTree();

		PeachNode* 
			FindChild(string fp_DesiredPeachNode); //returns first instance of child found matching the name

		PeachNode 
			Replicate();
	};

}