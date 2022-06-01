//
// GameSencha, LLC 6/1/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API ParserForNode : public ParserNode
{
	SYMPL_OBJECT(ParserForNode, ParserNode)

public:
	// Value for the start of the loop.
	SharedPtr<ParserNode> StartValueNode;
	// Goal value to break the loop.
	SharedPtr<ParserNode> EndValueNode;
	// Step value to increment by in the loop.
	SharedPtr<ParserNode> StepValueNode;
	// Body to execute for the loop.
	SharedPtr<ParserNode> BodyNode;

	/**
	 * Overrides the construct.
	 * @param argc
	 * @param ArgList
	 */
	virtual void __Construct(int argc, va_list ArgList) override;

	/**
	 * Constructor
	 * @param pNodeToken
	 * @param pStartValue
	 * @param pEndValue
	 * @param pStepValue
	 * @param pBody
	 */
	void Create(
			const SharedPtr<class Token>& pNodeToken,
			const SharedPtr<ParserNode>& pStartValue,
			const SharedPtr<ParserNode>& pEndValue,
			const SharedPtr<ParserNode>& pStepValue,
			const SharedPtr<ParserNode>& pBody
	);

	/**
	 * Returns the string representation of the node.
	 * @return
	 */
	virtual CStrPtr ToString() override;
};

SymplNamespaceEnd