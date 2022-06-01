//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "ValueHandle.hpp"

SymplNamespaceStart

class SYMPL_API NumberHandle : public ValueHandle
{
	SYMPL_OBJECT(NumberHandle, ValueHandle)

public:
    /**
     * Returns a new number that adds another number.
     * @param handle
     * @return
     */
    virtual SharedPtr<NumberHandle> AddTo(const SharedPtr<NumberHandle>& handle) = 0;

    /**
     * Returns a new number that's subtracted by the current value.
     * @param handle
     * @return
     */
    virtual SharedPtr<NumberHandle> SubtractBy(const SharedPtr<NumberHandle>& handle) = 0;

    /**
     * Returns a new number that's multiplied by the current value.
     * @param handle
     * @return
     */
    virtual SharedPtr<NumberHandle> MultiplyBy(const SharedPtr<NumberHandle>& handle) = 0;

    /**
     * Returns a new number that's divided by the current value.
     * @param handle
     * @return
     */
    virtual SharedPtr<NumberHandle> DivideBy(const SharedPtr<NumberHandle>& handle) = 0;

    /**
     * Returns a new number that's the power by a given value.
     * @param handle
     * @return
     */
    virtual SharedPtr<NumberHandle> PowerBy(const SharedPtr<NumberHandle>& handle) = 0;

    /**
     * Sets the integer value.
     * @param pValue
     */
    inline void SetIntValue(long pValue) { Value.IntNum = pValue; }

    /**
     * Returns the int value.
     * @return
     */
    inline long GetIntValue() const { return Value.IntNum; }

    /**
     * Sets the float value.
     * @param pValue
     */
    inline void SetFloatValue(float pValue) { Value.FloatNum = pValue; }

    /**
     * Returns the float value.
     * @return
     */
    inline float GetFloatValue() const { return Value.FloatNum; }

	/**
	 * Returns if the value is true.
	 * @return
	 */
	bool IsTrue() const override;
};

SymplNamespaceEnd
