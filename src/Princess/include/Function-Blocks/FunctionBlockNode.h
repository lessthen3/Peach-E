#pragma once

#include "../BlockNode.h"

namespace Princess {
	struct FunctionBlockNode : public BlockNode
	{
	public:
		FunctionBlockNode(const std::string& fp_Name, const unsigned int fp_ID): BlockNode(fp_ID)
		{
			m_Name = fp_Name; //name actually matters here hehehe xd
		}

	public:
		std::string ToScript(unsigned int depth = 0) const override {
			std::string indent(depth * 4, ' '); // 4 spaces per indent level
			return indent + "def " + m_Name + ":";
		}
	};
}

