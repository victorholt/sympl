//
// GameSencha, LLC 6/1/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "ParserNode.hpp"

SymplNamespaceStart

class SYMPL_API ParserWhileNode : public ParserNode
{
	SYMPL_OBJECT(ParserWhileNode, ParserNode)

public:
	// Goal value to break the loop.
	SharedPtr<ParserNode> ConditionNode;
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
	 * @param pCondition
	 * @param pBody
	 */
	void Create(
			const SharedPtr<class Token>& pNodeToken,
			const SharedPtr<ParserNode>& pCondition,
			const SharedPtr<ParserNode>& pBody
	);

	/**
	 * Returns the string representation of the node.
	 * @return
	 */
	virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
