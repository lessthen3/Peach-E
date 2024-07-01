#pragma once

#include "../BlockNode.h"

namespace Princess {


	struct WhileLoopBlockNode: public BlockNode
	{
	public:
		WhileLoopBlockNode(const std::string& fp_Name, const std::string& fp_CodeSnippet, const unsigned int fp_ID): BlockNode(fp_ID)
		{
			m_Name = "while";
		}

	public:
		std::string ToScript(unsigned int depth = 0) const override {
			std::string indent(depth * 4, ' '); // 4 spaces per indent level
			return indent + "while " + m_CodeSnippet + ":" + "\n";
		}
	};
}
