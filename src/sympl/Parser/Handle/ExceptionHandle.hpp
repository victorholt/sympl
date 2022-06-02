//
// GameSencha, LLC 6/2/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "ValueHandle.hpp"

SymplNamespaceStart

class SYMPL_API ExceptionHandle : public ValueHandle
{
	SYMPL_OBJECT(ExceptionHandle, ValueHandle)

public:
	// Exception error.
	SharedPtr<class RuntimeError> Error;

	/**
	 * Overrides the construct.
	 * @param argc
	 * @param ArgList
	 */
	void __Construct(int argc, va_list ArgList) override;

	/**
	 * Creates the handle.
	 * @param pError
	 */
	void Create(const SharedPtr<class RuntimeError>& pError);

	/**
     * Copies the value.
     * @return
     */
	[[nodiscard]]
	SharedPtr<ValueHandle> Copy() const override;

	/**
	 * Returns a string of the value.
	 * @return
	 */
	CStrPtr ToString() override;
};

SymplNamespaceEnd
