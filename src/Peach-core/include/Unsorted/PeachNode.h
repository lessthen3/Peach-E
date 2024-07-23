#pragma once

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

		virtual void Initialize() = 0;
		virtual void Update(float fp_TimeSinceLastFrame) = 0;
		virtual void ConstantUpdate(float fp_TimeSinceLastFrame) = 0;
		//virtual void HalfConstantUpdate(float fp_TimeSinceLastFrame);
		virtual void OnSceneTreeExit() = 0;
		virtual void Draw() = 0;

		bool IsPausable = true;
		bool IsVisible = true;
		bool ShouldBeInTree = false;

		vector<unique_ptr<PeachNode>> m_Children;

		PeachNode GetPeachNode();

		PeachNode ReparentPeachNode();

		virtual void QueueRemoval(); //queues for removal from scene tree at end of frame or whenever is convenient idk

		void PrintTree();

		bool IsInsideTree();

		bool HasPeachNode();

		void GetViewPort();

		string GetPathInTree();

		PeachNode* FindChild(string fp_DesiredPeachNode); //returns first instance of child found matching the name

		PeachNode Replicate();



	};

}