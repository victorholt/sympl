//
// GameSencha, LLC 6/1/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API ParserCallNode : public ParserNode
{
    SYMPL_OBJECT(ParserCallNode, ParserNode)

public:
    // Node to call.
    SharedPtr<ParserNode> CallNode;
    // List of arguments for the call.
    std::vector<SharedPtr<ParserNode>> ArgNodeList;

    /**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
    void __Construct(int argc, va_list ArgList) override;

    /**
     * Destruct.
     */
    void __Destruct() override;

    /**
     * Constructor
     * @param pNodeToken
     * @param pCallNode
     * @param pArgNodeList
     */
    void Create(
        const SharedPtr<class Token>& pNodeToken,
        const SharedPtr<ParserNode>& pCallNode,
        const std::vector<SharedPtr<ParserNode>>& pArgNodeList
    );

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
