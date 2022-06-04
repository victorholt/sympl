//
// GameSencha, LLC 5/26/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "NumberHandle.hpp"

SymplNamespaceStart

class SYMPL_API IntHandle : public NumberHandle
{
	SYMPL_OBJECT(IntHandle, NumberHandle)

public:
    /**
     * Constructor.
     */
    IntHandle();

    /**
     * Returns a new number that adds another number.
     * @param handle
     * @return
     */
    virtual SharedPtr<ValueHandle> AddTo(const SharedPtr<ValueHandle>& handle) override;

    /**
     * Returns a new number that's subtracted by the current value.
     * @param handle
     * @return
     */
    virtual SharedPtr<ValueHandle> SubtractBy(const SharedPtr<ValueHandle>& handle) override;

    /**
     * Returns a new number that's multiplied by the current value.
     * @param handle
     * @return
     */
    virtual SharedPtr<ValueHandle> MultiplyBy(const SharedPtr<ValueHandle>& handle) override;

    /**
     * Returns a new number that's divided by the current value.
     * @param handle
     * @return
     */
    virtual SharedPtr<ValueHandle> DivideBy(const SharedPtr<ValueHandle>& handle) override;

    /**
     * Returns a new number that's the power by a given value.
     * @param handle
     * @return
     */
    virtual SharedPtr<ValueHandle> PowerBy(const SharedPtr<ValueHandle>& handle) override;

    /**
     * Copies the value to a new value handle.
     * @return
     */
    [[nodiscard]]
    SharedPtr<ValueHandle> Copy() const override;

    /**
     * Returns a string of the value.
     * @return
     */
    CStrPtr ToString() override;

    /**
     * Sets the float value.
     * @param pValue
     */
    inline void SetValue(long pValue) { Value.IntNum = pValue; NormalizeValue(); }

    /**
     * Returns the value of the number.
     * @return
     */
    inline int GetValue() const { return Value.IntNum; }
};

SymplNamespaceEnd
