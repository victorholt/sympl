//
// GameSencha, LLC 5/26/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "NumberHandle.hpp"

SymplNamespaceStart

class IntHandle : public NumberHandle
{
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
    virtual SharedPtr<NumberHandle> AddTo(const SharedPtr<NumberHandle>& handle) override;

    /**
     * Returns a new number that's subtracted by the current value.
     * @param handle
     * @return
     */
    virtual SharedPtr<NumberHandle> SubtractBy(const SharedPtr<NumberHandle>& handle) override;

    /**
     * Returns a new number that's multiplied by the current value.
     * @param handle
     * @return
     */
    virtual SharedPtr<NumberHandle> MultiplyBy(const SharedPtr<NumberHandle>& handle) override;

    /**
     * Returns a new number that's divided by the current value.
     * @param handle
     * @return
     */
    virtual SharedPtr<NumberHandle> DivideBy(const SharedPtr<NumberHandle>& handle) override;

    /**
     * Returns a new number that's the power by a given value.
     * @param handle
     * @return
     */
    virtual SharedPtr<NumberHandle> PowerBy(const SharedPtr<NumberHandle>& handle) override;

    /**
     * Returns a string of the value.
     * @return
     */
    virtual CStrPtr ToString() override;

    /**
     * Sets the float value.
     * @param pValue
     */
    inline void SetValue(int pValue) { Value.IntNum = pValue; NormalizeValue(); }

    /**
     * Returns the value of the number.
     * @return
     */
    inline int GetValue() const { return Value.IntNum; }
};

SymplNamespaceEnd
