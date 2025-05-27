#pragma once


#include <string>
#include <vector>
#include <memory>

using namespace std;

namespace PeachCore {

	class PeachNode
	{
	public:
		virtual ~PeachNode();

        PeachNode(const string& fp_Name) 
        {
            m_Name = fp_Name;
        }

		PeachNode() {} //??????????????????

		bool IsPausable = true;
		bool IsVisible = true;
		bool IsActive = true;

        string m_Name; //this is the name set by the user inside the scene tree, so that a user script can call smth like GetNode("MyNamedNode") and find the appropriate thing

		unsigned int m_DrawingLayer = 0;

		vector<unique_ptr<PeachNode>> m_Children;

		PeachNode* GetPeachNode(); //returns a reference to the desired PeachNode

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