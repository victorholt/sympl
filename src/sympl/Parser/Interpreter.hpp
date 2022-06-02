//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/Memory/ManagedObject.hpp>

SymplNamespaceStart

class SYMPL_API Interpreter : public ManagedObject
{
	SYMPL_OBJECT(Interpreter, ManagedObject)

public:
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
     * Visits the if node.
     * @param Node
     * @param Context
     * @return
     */
	SharedPtr<class ParserRuntimeResult> VisitIfNode(SharedPtr<class ParserNode> Node, SharedPtr<ParserContext> Context);

	/**
     * Visits the for node.
     * @param Node
     * @param Context
     * @return
     */
	SharedPtr<class ParserRuntimeResult> VisitForNode(SharedPtr<class ParserNode> Node, SharedPtr<ParserContext> Context);

	/**
     * Visits the while node.
     * @param Node
     * @param Context
     * @return
     */
	SharedPtr<class ParserRuntimeResult> VisitWhileNode(SharedPtr<class ParserNode> Node, SharedPtr<ParserContext> Context);

    /**
     * Visits the variable access node.
     * @param Node
     * @param Context
     * @return
     */
    SharedPtr<class ParserRuntimeResult> VisitVarAccessNode(SharedPtr<class ParserNode> Node, SharedPtr<ParserContext> Context);

    /**
     * Visits the variable assign node.
     * @param Node
     * @param Context
     * @return
     */
    SharedPtr<class ParserRuntimeResult> VisitVarAssignNode(SharedPtr<class ParserNode> Node, SharedPtr<ParserContext> Context);

    /**
     * Visits the function definition node.
     * @param Node
     * @param Context
     * @return
     */
    SharedPtr<class ParserRuntimeResult> VisitFuncDefNode(SharedPtr<class ParserNode> Node, SharedPtr<ParserContext> Context);

    /**
     * Visits the call node.
     * @param Node
     * @param Context
     * @return
     */
    SharedPtr<class ParserRuntimeResult> VisitCallNode(SharedPtr<class ParserNode> Node, SharedPtr<ParserContext> Context);

    /**
     * Failed to visit a node.
     * @param Node
     * @param Context
     * @return
     */
    SharedPtr<class ParserRuntimeResult> NoVisit(SharedPtr<class ParserNode> Node, SharedPtr<ParserContext> Context);
};

SymplNamespaceEnd
