#pragma once

#include <vector>
#include "../BlockNode.h"

namespace Princess {


	class ExecutionParser
	{
	public:
		std::vector<BlockNode> m_BlockExecutionOrder; //stores all the blocks that are to be executed for when they're glued back together into a plain old .py file and their branches 
	
	private:


	};
}
