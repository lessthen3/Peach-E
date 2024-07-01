////////////////////////////////////////////////////////
/***                             [Princess]                                     ***/
////////////////////////////////////////////////////////
/***                            [Version 0.0.01]                             ***/
////////////////////////////////////////////////////////
/***  Copyright(c) 2024-present Ranyodh Singh Mandur  ***/
/***                               MIT License                                ***/
/*** Documentation:TBD                                                   ***/
/*** GitHub: https://github.com/iLoveJohnFish/Peach-E ***/
/////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include "../BlockNode.h"

namespace Princess {

    struct ExecutionParser {

    public:
        static ExecutionParser& Parser() {
            static ExecutionParser instance;
            return instance;
        }

    public:
        std::vector<std::unique_ptr<BlockNode>>& GetRootLevelBlockNodeExecutionOrder();

        void FindAndRemoveBlockNode(BlockNode* fp_NodeToBeRemoved);

        void FlagBlockNodeForRootLevelExecution(const unsigned int fp_BlockNodeID);
        void RemoveFlagFromBlockNodeForRootLevelExecution(const unsigned int fp_BlockNodeID);

        void ReparentBlockNode();

        void ExecuteScript();

    public:

    private:
        ExecutionParser() = default;
        ~ExecutionParser() = default;

        ExecutionParser(const ExecutionParser&) = delete;
        ExecutionParser& operator=(const ExecutionParser&) = delete;

    private:
        void DFS(const BlockNode* fp_Node, unsigned int fp_Depth, std::string& fp_Script) const;
        BlockNode* DFSFindBlockNodeReferenceInTree(const unsigned int fp_BlockNodeID, const bool fp_IsLookingForRoot = true);
        BlockNode* DFSReturnSpecificBlockNodePointerReference(BlockNode* fp_BlockNode, const unsigned int fp_BlockNodeID);

        void MovePointerReferencesToRootLevelBlockNodeExecutionOrder();

        void SortRootLevelBlockNodeExecutionOrder();
        bool CheckIfSorted();

        void MovePointerReferencesBackToAllCurrentlyPlacedBlockNodes();

        void RunPythonScript();

        std::string GenerateScript(const BlockNode* fp_StartingBlockNode) const;

	private:
		std::vector<std::unique_ptr<BlockNode>> pm_RootLevelBlockNodeExecutionOrder = {}; //stores all the blocks that are to be executed for when they're glued back together into a plain old .py file and their branches 
        std::vector<std::unique_ptr<BlockNode>> pm_AllCurrentlyPlacedBlockNodes = {};

        std::string pm_CurrentScript;
    };
}
