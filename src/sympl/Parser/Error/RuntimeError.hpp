//
// GameSencha, LLC 5/26/22.
//
#pragma once
#include "ParserError.hpp"

SymplNamespaceStart

class RuntimeError : public ParserError
{
public:

    /**
     * Constructor.
     * @param StartPosition
     * @param EndPosition
     * @param ErrorDetails
     */
    RuntimeError(const SharedPtr<class LexerPosition>& StartPosition, const SharedPtr<class LexerPosition>& EndPosition, CStrPtr ErrorDetails);
};

SymplNamespaceEnd
