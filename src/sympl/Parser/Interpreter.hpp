//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/Memory/ManagedObject.hpp>

SymplNamespaceStart

class Interpreter : public ManagedObject
{
public:
    Interpreter() = default;

    /**
     * Visits the parser nodes.
     * @param Node
     * @param Context
     * @return
     */
    SharedPtr<class ParserRuntimeResult> Visit(SharedPtr<class ParserNode> Node, SharedPtr<class ParserContext> Context);

    /**
     * Visits the parser number node.
     * @param Node
     * @param Context
     * @return
     */
    SharedPtr<class ParserRuntimeResult> VisitNumberNode(SharedPtr<class ParserNode> Node, SharedPtr<ParserContext> Context);

    /**
     * Visits the parser binary node.
     * @param Node
     * @param Context
     * @return
     */
    SharedPtr<class ParserRuntimeResult> VisitBinaryOpNode(SharedPtr<class ParserNode> Node, SharedPtr<ParserContext> Context);

    /**
     * Visits the unary operator node.
     * @param Node
     * @param Context
     * @return
     */
    SharedPtr<class ParserRuntimeResult> VisitUnaryOpNode(SharedPtr<class ParserNode> Node, SharedPtr<ParserContext> Context);

    /**
     * Failed to visit a node.
     * @param Node
     * @param Context
     * @return
     */
    SharedPtr<class ParserRuntimeResult> NoVisit(SharedPtr<class ParserNode> Node, SharedPtr<ParserContext> Context);
};

SymplNamespaceEnd
