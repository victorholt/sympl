//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include "ValueHandle.hpp"

SymplNamespaceStart

class SYMPL_API NumberHandle : public ValueHandle
{
	SYMPL_OBJECT(NumberHandle, ValueHandle)

public:
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
