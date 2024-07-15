#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../Unsorted/PeachObject.h"

namespace PeachCore {

	class PeachNode: public PeachObject 
	{
	public:
		PeachNode() {

		}
		virtual ~PeachNode();

		virtual void Initialize();
		virtual void Update(float fp_TimeSinceLastFrame);
		virtual void ConstantUpdate(float fp_TimeSinceLastFrame);
		//virtual void HalfConstantUpdate(float fp_TimeSinceLastFrame);
		virtual void OnSceneTreeExit();

		bool IsPausable = true;
		bool IsVisible = true;
		bool ShouldBeInTree = false;

		std::vector<std::unique_ptr<PeachNode>> m_Children;

		PeachNode GetPeachNode();

		PeachNode ReparentPeachNode();

		virtual void QueueRemoval(); //queues for removal from scene tree at end of frame or whenever is convenient idk

		void PrintTree();

		bool IsInsideTree();

		bool HasPeachNode();

		void GetViewPort();

		std::string GetPathInTree();

		PeachNode* FindChild(std::string fp_DesiredPeachNode); //returns first instance of child found matching the name

		PeachNode Replicate();



	};

}