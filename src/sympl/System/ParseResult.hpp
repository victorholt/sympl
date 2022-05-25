//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include "SymplPCH.hpp"
#include "SymplError.hpp"
#include <sympl/Memory/ManagedObject.hpp>

SymplNamespaceStart

class ParseResult : public ManagedObject
{
public:
	// Reference to the error.
	SharedPtr<SymplError> Error;

	// Reference to the parser node.
	SharedPtr<ManagedObject> ParserNode;

	/**
	 * Registers a parse result.
	 * @param Result
	 * @return
	 */
	SharedPtr<ManagedObject> RegisterResult(SharedPtr<ParseResult> Result);

	/**
	 * Registers a parse node.
	 * @param Node
	 * @return
	 */
	SharedPtr<ManagedObject> RegisterNode(SharedPtr<ManagedObject> Node);

	/**
	 * Handles the success.
	 * @param Node
	 */
	void Success(const SharedPtr<ManagedObject>& Node);

	/**
	 * Handles a failure.
	 * @param pError
	 */
	void Failure(const SharedPtr<SymplError>& pError);

};

SymplNamespaceEnd