//
// GameSencha, LLC 6/2/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "ValueHandle.hpp"

SymplNamespaceStart

class SYMPL_API ListHandle : public ValueHandle
{
    SYMPL_OBJECT(ListHandle, ValueHandle)

public:
    // List elements.
    std::vector<SharedPtr<ValueHandle>> Elements;

    /**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
    void __Construct(int argc, va_list ArgList) override;

    /**
     * Creates the list.
     */
    void Create(const std::vector<SharedPtr<ValueHandle>>& pElements);

    /**
     * Returns a new number that adds another number.
     * @param pHandle
     * @return
     */
    SharedPtr<ValueHandle> AddTo(const SharedPtr<ValueHandle>& pHandle) override;

	/**
     * Returns a new number that's subtracted by the current value.
     * @param handle
     * @return
     */
	SharedPtr<ValueHandle> SubtractBy(const SharedPtr<ValueHandle>& pHandle) override;

    /**
     * Returns a new number that's multiplied by the current value.
     * @param pHandle
     * @return
     */
    SharedPtr<ValueHandle> MultiplyBy(const SharedPtr<ValueHandle>& pHandle) override;

	/**
     * Returns a new number that's divided by the current value.
     * @param handle
     * @return
     */
	SharedPtr<ValueHandle> DivideBy(const SharedPtr<ValueHandle>& pHandle) override;

    /**
     * Attempts to normalize the value.
     */
    void NormalizeValue() override;

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
