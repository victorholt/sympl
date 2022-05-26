//
// GameSencha, LLC 5/26/22.
//
#pragma once
#include "ParserError.hpp"

SymplNamespaceStart

class RuntimeError : public ParserError
{
protected:
    // Start position.
    SharedPtr<class LexerPosition> StartPosition;
    // End position.
    SharedPtr<class LexerPosition> EndPosition;

public:
    // Reference to the context.
    SharedPtr<class ParserContext> Context;

    /**
     * Constructor.
     * @param pContext
     * @param pStartPosition
     * @param pEndPosition
     * @param ErrorDetails
     */
    RuntimeError(
        const SharedPtr<class ParserContext>& pContext,
        const SharedPtr<class LexerPosition>& pStartPosition,
        const SharedPtr<class LexerPosition>& pEndPosition,
        CStrPtr ErrorDetails
    );

    /**
     * Generates a traceback string.
     * @return
     */
    SharedPtr<class StringBuffer> GenerateTraceback();

    /**
	 * Returns string of the error.
	 * @return
	 */
    virtual CStrPtr ToString() override;
};

SymplNamespaceEnd
