//
// GameSencha, LLC 6/1/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include "ValueHandle.hpp"

SymplNamespaceStart

class SYMPL_API NullHandle : public ValueHandle
{
	SYMPL_OBJECT(NullHandle, ValueHandle)

public:

	/**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
	void __Construct(int argc, va_list ArgList) override;

	/**
	 * Creates the handle.
	 */
	void Create();

	/**
	 * Returns if the value is true.
	 * @return
	 */
	bool IsTrue() const override;

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
