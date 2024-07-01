/*
	This class is used to
	Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

#pragma once

#include <vector>
#include <memory>

#include <string>

namespace Princess {

	struct BlockNode
	{
	public:
		std::string m_Name;
		std::string m_CodeSnippet;
		int m_LineNumber = -1;
		unsigned int m_InputLineNumber; //will start at lineNumber 0 originating from the "Program Enter" node, dictates order
		const unsigned int m_ID;
		int m_ParentID = -1; //initialize to -1 to indicate no parent , id's are strictly positive
		bool m_IsRootExecutable = false;

		std::vector<std::unique_ptr<BlockNode>> m_Children;

	public:
		BlockNode(const std::string& fp_Name, const std::string& fp_CodeSnippet, const unsigned int fp_ID)
			: m_Name(fp_Name), m_CodeSnippet(fp_CodeSnippet), m_ID(fp_ID) {}

		BlockNode(const std::string& fp_Name, const unsigned int fp_ID)
			: m_Name(fp_Name), m_ID(fp_ID) {}

		BlockNode(const unsigned int fp_ID)
			: m_ID(fp_ID) {}

		virtual ~BlockNode() = default;

	public:
		virtual std::string ToScript(unsigned int depth = 0) const{
			std::string indent(depth * 4, ' '); // 4 spaces per indent level
			return indent + m_CodeSnippet + "\n";
		}
	};
}
