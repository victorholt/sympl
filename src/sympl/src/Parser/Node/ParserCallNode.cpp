//
// GameSencha, LLC 6/1/22.
//
#include <sympl/include/Parser/Node/ParserCallNode.hpp>
#include <sympl/include/Parser/Token.hpp>
SymplNamespace

void ParserCallNode::__Construct(int argc, va_list ArgList)
{
}

void ParserCallNode::__Destruct()
{
    ParserNode::__Destruct();

    CallNode = nullptr;
    ArgNodeList.clear();
}

void ParserCallNode::Create(
    const SharedPtr<Token>& pNodeToken,
    const SharedPtr<ParserNode>& pCallNode,
    const std::vector<SharedPtr<ParserNode>>& pArgNodeList
)
{
    ParserNode::Create(ParseNodeType::Call, pNodeToken);

    CallNode = pCallNode;
    ArgNodeList = pArgNodeList;

    StartPosition = CallNode->GetStartPosition();

    if (!ArgNodeList.empty()) {
        EndPosition = ArgNodeList[ArgNodeList.size() - 1]->GetEndPosition();
    }
    else {
        EndPosition = CallNode->GetEndPosition();
    }
}

CStrPtr ParserCallNode::ToString()
{
    memset(TmpNodeString_Allocate, 0, sizeof(TmpNodeString_Allocate));
    strcpy(TmpNodeString_Allocate, fmt::format(
            "({0})",
            NodeToken->ToString()
    ).c_str());

    return TmpNodeString_Allocate;
}
