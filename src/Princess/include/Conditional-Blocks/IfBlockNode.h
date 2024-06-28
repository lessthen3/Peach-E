#pragma once

#include "../BlockNode.h"

namespace Princess
{

	struct IfBlockNode: public BlockNode
	{
	public:
		IfBlockNode(const std::string& fp_CodeSnippet, const unsigned int fp_ID): BlockNode(fp_ID)
		{
			m_Name = "if";
		}

	public:
		std::string ToScript(unsigned int depth = 0) const override {
			std::string indent(depth * 4, ' '); // 4 spaces per indent level
			return indent + "if " + m_CodeSnippet + ":";
		}
	};
}
