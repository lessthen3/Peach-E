#pragma once

#include "../BlockNode.h"

namespace Princess {
	struct FunctionDefinitionBlockNode : public BlockNode
	{
	public:
		FunctionDefinitionBlockNode(const unsigned int fp_ID): BlockNode(fp_ID)
		{
			
		}

	public:
		std::string ToScript(unsigned int depth = 0) const override {
			std::string indent(depth * 4, ' '); // 4 spaces per indent level
			return indent + "def " + m_Name + ":" + "\n";
		}
	};
}

