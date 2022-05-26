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
