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
#include "../../include/Parsers/ExecutionParser.h"
#include "../../../Peach-core/include/Managers/LogManager.h"

using namespace std;

namespace Princess {


    string 
        ExecutionParser::GenerateScript(const BlockNode* fp_StartingBlockNode) 
        const
    {
        string f_PythonScript;
        DFS(fp_StartingBlockNode, 0, f_PythonScript);
        PeachCore::LogManager::Logger().Debug(" ", "ExecutionParser");
        return f_PythonScript;
    }

    //DFS used to build script when ready from root node BlockNodes, strategy is to iterate through all block nodes, run DFS on flagged block nodes
    void 
        ExecutionParser::DFS(const BlockNode* fp_Node, unsigned int fp_Depth, string& fp_Script) 
        const
    {
        if (!fp_Node) { return; }
        fp_Script += fp_Node->ToScript(fp_Depth);

        for (const auto& child : fp_Node->m_Children) 
        {
            DFS(child.get(), fp_Depth + 1, fp_Script);  // child.get() returns a raw pointer
        }
    }

    //Searches a specific node reference's children all the way down to see if desire BlockNodeID is found, if not found it returns nullptr
    BlockNode* 
        ExecutionParser::DFSReturnSpecificBlockNodePointerReference(BlockNode* fp_BlockNode, const unsigned int fp_BlockNodeID)
    {
        if (!fp_BlockNode) { return nullptr; }
        if (fp_BlockNode->m_ID == fp_BlockNodeID) { return fp_BlockNode; }

        for (const auto& child : fp_BlockNode->m_Children) 
        {
            BlockNode* result = DFSReturnSpecificBlockNodePointerReference(child.get(), fp_BlockNodeID);
            if (result) { return result; }
        }
        return nullptr;
    }

    //Iterates through list of all placed block nodes and applies DFSReturnBlockNodePointerReference() to each root block of the scene and returns nullptr if not found
    BlockNode* 
        ExecutionParser::DFSFindBlockNodeReferenceInTree(unsigned int fp_BlockNodeID, bool fp_IsLookingForRoot) 
    {
        for (const auto& l_BlockNode : pm_AllCurrentlyPlacedBlockNodes)
        {
            if (!l_BlockNode->m_IsRootExecutable && fp_IsLookingForRoot) 
            {
                continue;
            }

            BlockNode* result = DFSReturnSpecificBlockNodePointerReference(l_BlockNode.get(), fp_BlockNodeID);

            if (result) 
            {
                return result;
            }
        }

        return nullptr;
    }


    void 
        ExecutionParser::FindAndRemoveBlockNode(BlockNode* fp_NodeToBeRemoved)
    {
        if (!fp_NodeToBeRemoved)
        {
            PeachCore::LogManager::Logger().Warn("Invalid input for 'FindAndRemoveBlockNode()", "ExecutionParser");
            return;
        }

        BlockNode* f_ParentOfNodeToBeRemoved = DFSFindBlockNodeReferenceInTree(fp_NodeToBeRemoved->m_ParentID, false);

        if (!f_ParentOfNodeToBeRemoved) 
        {
            PeachCore::LogManager::Logger().Debug("Parent Node with ID:  could not be found", "ExecutionParser");
            return;
        }

        auto& children = f_ParentOfNodeToBeRemoved->m_Children;

        children.erase(remove_if(children.begin(), children.end(),
            [fp_NodeToBeRemoved](const unique_ptr<BlockNode>& child) {
                return child.get() == fp_NodeToBeRemoved;
            }), children.end());

        PeachCore::LogManager::Logger().Debug("Successfully removed BlockNode: " + fp_NodeToBeRemoved->m_Name, "ExecutionParser");
    }


    void 
        ExecutionParser::ExecuteScript()
    {
        MovePointerReferencesToRootLevelBlockNodeExecutionOrder();
        SortRootLevelBlockNodeExecutionOrder();

        // Execute Script
        RunPythonScript();

        MovePointerReferencesBackToAllCurrentlyPlacedBlockNodes();
    }


    void 
        ExecutionParser::MovePointerReferencesToRootLevelBlockNodeExecutionOrder()
    {
        for (int i = pm_AllCurrentlyPlacedBlockNodes.size() - 1; i >= 0; --i) 
        {
            if (pm_AllCurrentlyPlacedBlockNodes[i]->m_IsRootExecutable) 
            {
                pm_RootLevelBlockNodeExecutionOrder.push_back(move(pm_AllCurrentlyPlacedBlockNodes[i]));
                pm_AllCurrentlyPlacedBlockNodes.erase(pm_AllCurrentlyPlacedBlockNodes.begin() + i);
            }
        }
    }


    void 
        ExecutionParser::SortRootLevelBlockNodeExecutionOrder()
    {
        sort(
            pm_RootLevelBlockNodeExecutionOrder.begin(), pm_RootLevelBlockNodeExecutionOrder.end(),
            [](const unique_ptr<BlockNode>& a, const unique_ptr<BlockNode>& b)
            {
                return a->m_InputLineNumber < b->m_InputLineNumber;
            });
    }


    bool 
        ExecutionParser::CheckIfSorted()
    {
        for (int i = 0; i < (pm_AllCurrentlyPlacedBlockNodes.size() - 1) - 1; i++) // -1 again so that we dont go out of bounds for index checking with i + 1
        {
            if(pm_RootLevelBlockNodeExecutionOrder[i]->m_InputLineNumber > pm_RootLevelBlockNodeExecutionOrder[i + 1]->m_InputLineNumber) //if sorted, index i should be equal to the m_InputLineNumber up to a constant
            {
                return false;
            }
        }
        return true;
    }


    void 
        ExecutionParser::MovePointerReferencesBackToAllCurrentlyPlacedBlockNodes()
    {

    }


    void 
        ExecutionParser::RunPythonScript()
    {

    }
}



//for (const auto& l_Child : l_BlockNode->m_Children) {
//    f_TempBlockNodeReference = DFSReturnPointerReferenceOfDesiredBlockNodeID(l_Child->m_ID);
//    if ()
//}

//vector<unique_ptr<BlockNode>>& ExecutionParser::GetRootLevelBlockNodeExecutionOrder() {
//    return pm_RootLevelBlockNodeExecutionOrder;
//}