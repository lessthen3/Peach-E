#pragma once

/*
    This class is used to
    Copyright(c) 2024-present Ranyodh Singh Mandur.

*/


#include <vector>
#include "../BlockNode.h"

namespace Princess {


    class ExecutionParser {
    public:
        static ExecutionParser& Parser() {
            static ExecutionParser instance;
            return instance;
        }

    public:
        std::vector<BlockNode>& GetBlockExecutionOrder();

    private:
        ExecutionParser() = default;
        ~ExecutionParser() = default;

        ExecutionParser(const ExecutionParser&) = delete;
        ExecutionParser& operator=(const ExecutionParser&) = delete;
	
	private:
		std::vector<BlockNode> pm_BlockExecutionOrder = {}; //stores all the blocks that are to be executed for when they're glued back together into a plain old .py file and their branches 

	};
}
