#pragma once

#include "../BlockNode.h"

namespace Princess {

	struct BreakBlockNode: public BlockNode
	{
	public:
		BreakBlockNode(const unsigned int fp_ID): BlockNode(fp_ID) 
		{
			m_Name = "break";
			m_CodeSnippet = "break";
		}

		std::string ToScript(unsigned int depth = 0) const override {
			std::string indent(depth * 4, ' '); // 4 spaces per indent level
			return indent + "break" + "\n";
		}
	};
}
