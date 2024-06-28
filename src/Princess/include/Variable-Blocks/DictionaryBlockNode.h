#pragma once

/*
	This class is used to
	Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

#include "../BlockNode.h"

namespace Princess {

	struct DictionaryBlockNode : public BlockNode
	{
	public:
		DictionaryBlockNode(const std::string& fp_Name, const unsigned int fp_ID): BlockNode(fp_ID)
		{
			m_Name = fp_Name;
		}

	public:
		std::string ToScript(unsigned int depth = 0) const override {
			std::string indent(depth * 4, ' '); // 4 spaces per indent level
			return indent + m_Name + " = " + m_CodeSnippet;
		}
	};
}
