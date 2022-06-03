//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include "sympl/Parser/Error/ParserError.hpp"
#include <sympl/Memory/ManagedObject.hpp>

SymplNamespaceStart

class SYMPL_API ParserRuntimeResult : public ManagedObject
{
	SYMPL_OBJECT(ParserRuntimeResult, ManagedObject)

public:
    // Reference to the error.
    SharedPtr<ParserError> Error;

    // Reference to value handle.
    SharedPtr<class ValueHandle> Value;

    // Reference to the function return value.
    SharedPtr<class ValueHandle> FuncReturnValue;

    // Check if a loop should continue.
    bool LoopShouldContinue = false;
    // Check if a loop should break.
    bool LoopShouldBreak = false;

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
     * Handles the success.
     * @param pValue
     */
    void SuccessReturn(const SharedPtr<class ValueHandle>& pValue);

    /**
     * Handles the success.
     */
    void SuccessContinue();

    /**
     * Handles the success.
     */
    void SuccessBreak();

    /**
     * Handles a failure.
     * @param pError
     */
    void Failure(const SharedPtr<ParserError>& pError);

    /**
     * Check if we should return.
     * @return
     */
    bool ShouldReturn();

    /**
     * Resets the value.
     */
    void Reset();

};

SymplNamespaceEnd
