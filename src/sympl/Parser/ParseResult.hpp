//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include "SymplError.hpp"
#include <sympl/Memory/ManagedObject.hpp>

SymplNamespaceStart

class ParseResult : public ManagedObject
{
public:
	// Reference to the error.
	SharedPtr<SymplError> Error;

	// Reference to the parser node.
	SharedPtr<class ParserNode> ParserNodePtr;

	/**
	 * Handles the success.
	 * @param Node
	 */
	void Success(const SharedPtr<class ParserNode>& Node);

	/**
	 * Handles a failure.
	 * @param pError
	 */
	void Failure(const SharedPtr<SymplError>& pError);

};

SymplNamespaceEnd
