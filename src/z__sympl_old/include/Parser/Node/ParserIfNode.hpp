//
// GameSencha, LLC 6/1/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include "ParserNode.hpp"

SymplNamespaceStart

// Condition, Statement, ShouldReturnNull
typedef std::vector< std::tuple<SharedPtr<ParserNode>, SharedPtr<ParserNode>, bool> > IfCaseList;
typedef std::tuple<SharedPtr<ParserNode>, bool> ElseCaseTuple;

class SYMPL_API ParserIfNode : public ParserNode
{
	SYMPL_OBJECT(ParserIfNode, ParserNode)

public:
	// Tuple of conditions and expressions.
	IfCaseList Cases;

	// Else case to check.
    ElseCaseTuple ElseCase;

	/**
	 * Overrides the construct.
	 * @param argc
	 * @param ArgList
	 */
	virtual void __Construct(int argc, va_list ArgList) override;

	/**
	 * Constructor
	 * @param pNodeToken
	 * @param pCases
	 * @param pElseCases
	 */
	void Create(const SharedPtr<class Token>& pNodeToken, const IfCaseList& pCases, const ElseCaseTuple& pElseCase);

	/**
	 * Returns the string representation of the node.
	 * @return
	 */
	virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
