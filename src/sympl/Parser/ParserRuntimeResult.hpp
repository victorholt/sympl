//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include "sympl/Parser/Error/ParserError.hpp"
#include <sympl/Memory/ManagedObject.hpp>

SymplNamespaceStart

class ParserRuntimeResult : public ManagedObject
{
public:
    // Reference to the error.
    SharedPtr<ParserError> Error;

    // Reference to value handle.
    SharedPtr<class ValueHandle> Value;

    /**
     * Registers a result.
     * @param pResult
     */
    SharedPtr<class ValueHandle> Register(const SharedPtr<class ParserRuntimeResult>& pResult);

    /**
     * Handles the success.
     * @param pValue
     */
    void Success(const SharedPtr<class ValueHandle>& pValue);

    /**
     * Handles a failure.
     * @param pError
     */
    void Failure(const SharedPtr<ParserError>& pError);

};

SymplNamespaceEnd
