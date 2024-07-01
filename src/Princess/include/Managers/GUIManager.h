#pragma once

#include <map>
#include <vector>
#include "../BlockNode.h"
#include "../../include/UI-Elements/NodeConnectingLine.h"

namespace Princess {

	struct GUIManager {
	private:
		unsigned int IDCounter = 1;

		std::map<NodeConnectingLine, BlockNode> pm_MainLineOfExecution;


	};
}