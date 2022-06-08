//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/include/Parser/Error/ParserError.hpp>
#include <sympl/include/Memory/ManagedObject.hpp>

SymplNamespaceStart

class SYMPL_API ParseResult : public ManagedObject
{
	SYMPL_OBJECT(ParseResult, ManagedObject)

public:
	// Reference to the error.
	SharedPtr<ParserError> Error;

	// Reference to the parser node.
	SharedPtr<class ParserNode> ParserNodePtr;

    // Check if there has been an advancement.
    int AdvanceCount = 0;
    // Amount we should reverse the advancement.
    int ToReverseCount = 0;

    /**
     * Destruct.
     */
    void __Destruct() override;

    /**
     * Registers and returns a node.
     * @param Result
     * @return
     */
    SharedPtr<class ParserNode> Register(const SharedPtr<ParseResult>& Result);

    /**
     * Registers and returns a node.
     * @param Result
     * @return
     */
    SharedPtr<class ParserNode> TryRegister(const SharedPtr<ParseResult>& Result);

    /**
     * Handles incrementing the advance count.
     */
    void RegisterAdvance();

	/**
	 * Handles the success.
	 * @param Node
	 */
	void Success(const SharedPtr<class ParserNode>& Node);

	/**
	 * Handles a failure.
	 * @param pError
	 */
	void Failure(const SharedPtr<ParserError>& pError);

};

SymplNamespaceEnd
