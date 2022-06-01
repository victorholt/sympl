//
// GameSencha, LLC 5/31/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "ValueHandle.hpp"

SymplNamespaceStart

class SYMPL_API CompareHandle : public ValueHandle
{
	SYMPL_OBJECT(CompareHandle, ValueHandle)

public:
	// Result of the handle.
	bool Result = false;

	/**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
	void __Construct(int argc, va_list ArgList) override;

	/**
	 * Creates the handle.
	 * @param pType
	 * @param FromValue
	 */
	void Create(ValueType pType, SharedPtr<ValueHandle> FromValue);

	/**
	 * Creates a new compare handle from a value.
	 * @param Handle
	 * @return
	 */
	static SharedPtr<CompareHandle> CreateFrom(const SharedPtr<ValueHandle> Handle);

	/**
	 * Returns if two values are equal.
	 * @param handle
	 * @return
	 */
	virtual SharedPtr<CompareHandle> CompareEqual(const SharedPtr<CompareHandle>& handle);

	/**
	 * Returns if two values are not equal.
	 * @param handle
	 * @return
	 */
	virtual SharedPtr<CompareHandle> CompareNotEqual(const SharedPtr<CompareHandle>& handle);

	/**
	 * Returns if a value is less than another value.
	 * @param handle
	 * @return
	 */
	virtual SharedPtr<CompareHandle> CompareLessThan(const SharedPtr<CompareHandle>& handle);

	/**
	 * Returns if a value is less than or equal to another value.
	 * @param handle
	 * @return
	 */
	virtual SharedPtr<CompareHandle> CompareLessThanOrEqual(const SharedPtr<CompareHandle>& handle);

	/**
	 * Returns if a value is greater than another value.
	 * @param handle
	 * @return
	 */
	virtual SharedPtr<CompareHandle> CompareGreaterThan(const SharedPtr<CompareHandle>& handle);

	/**
	 * Returns if a value is greater than or equal to another value.
	 * @param handle
	 * @return
	 */
	virtual SharedPtr<CompareHandle> CompareGreaterThanOrEqual(const SharedPtr<CompareHandle>& handle);

	/**
	 * Returns if a value is not true.
	 * @return
	 */
	virtual SharedPtr<CompareHandle> CompareNot();

	/**
	 * Returns if a value and another value are true.
	 * @param handle
	 * @return
	 */
	virtual SharedPtr<CompareHandle> CompareAnd(const SharedPtr<CompareHandle>& handle);

	/**
	 * Returns if a value or another value or true.
	 * @param handle
	 * @return
	 */
	virtual SharedPtr<CompareHandle> CompareOr(const SharedPtr<CompareHandle>& handle);

	/**
	 * Returns if the value is true.
	 * @return
	 */
	bool IsTrue() const override;

	/**
     * Returns a string of the value.
     * @return
     */
	CStrPtr ToString() override;
};

SymplNamespaceEnd
