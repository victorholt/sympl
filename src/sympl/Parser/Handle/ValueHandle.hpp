//
// GameSencha, LLC 5/26/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/Memory/ManagedObject.hpp>
#include <sympl/Core/StringBuffer.hpp>

SymplNamespaceStart

class SYMPL_API ValueHandle : public ManagedObject
{
	SYMPL_OBJECT(ValueHandle, ManagedObject)

public:
	// Keep the type of number in the same memory location.
	struct ValueVariant
	{
		SharedPtr<StringBuffer> String;
		long IntNum = 0;
		float FloatNum = 0.f;

		ValueVariant& operator=(const ValueVariant& rhs) {
			if (this == &rhs) {
				return *this;
			}

			String = rhs.String;
			IntNum = rhs.IntNum;
			FloatNum = rhs.FloatNum;

			return *this;
		}
	};

	// Value of the handle.
	ValueVariant Value;

    // Type of value.
    ValueType Type = ValueType::Int;
    // Error from the value.
    SharedPtr<class ParserError> Error;
    // Context of the value.
    SharedPtr<class ParserContext> Context;

	// Check if we can assign a new value.
	bool Immutable = false;

protected:
	// String representation of the value.
	SharedPtr<StringBuffer> StringRep;

    // Start position of the number value.
    SharedPtr<class LexerPosition> StartPosition;
    // End position of the number value.
    SharedPtr<class LexerPosition> EndPosition;

public:
    /**
     * Constructor.
     */
    ValueHandle();

    /**
     * Sets the position of the number handle.
     * @param pStartPosition
     * @param pEndPosition
     */
    void SetPosition(const SharedPtr<class LexerPosition>& pStartPosition, const SharedPtr<class LexerPosition>& pEndPosition);

    /**
     * Returns a new number that adds another number.
     * @param handle
     * @return
     */
    virtual SharedPtr<ValueHandle> AddTo(const SharedPtr<ValueHandle>& pHandle);

    /**
     * Returns a new number that's subtracted by the current value.
     * @param handle
     * @return
     */
    virtual SharedPtr<ValueHandle> SubtractBy(const SharedPtr<ValueHandle>& pHandle);

    /**
     * Returns a new number that's multiplied by the current value.
     * @param handle
     * @return
     */
    virtual SharedPtr<ValueHandle> MultiplyBy(const SharedPtr<ValueHandle>& pHandle);

    /**
     * Returns a new number that's divided by the current value.
     * @param handle
     * @return
     */
    virtual SharedPtr<ValueHandle> DivideBy(const SharedPtr<ValueHandle>& pHandle);

    /**
     * Returns a new number that's the power by a given value.
     * @param handle
     * @return
     */
    virtual SharedPtr<ValueHandle> PowerBy(const SharedPtr<ValueHandle>& pHandle);

    /**
     * Creates a new context.
     * @param ContextName
     * @return
     */
    SharedPtr<class ParserContext> GenerateNewContext(
            CStrPtr ContextName,
            const SharedPtr<class ParserContext>& pParentContext = nullptr
    ) const;

	/**
     * Attempts to normalize the value.
     */
	virtual void NormalizeValue();

	/**
	 * Returns if the value is true.
	 * @return
	 */
	virtual bool IsTrue() const;

	/**
	 * Returns (typically) if this is a null handle.
	 * @return
	 */
	virtual bool IsNull() const;

    /**
     * Creates a copy of the value.
     * @return
     */
    template<class T>
    static SharedPtr<T> BaseCopy(const T* BaseHandle)
    {
        auto NewValue = ValueHandle::Alloc<T>();
        NewValue->Type = BaseHandle->Type;
        NewValue->SetPosition(BaseHandle->StartPosition, BaseHandle->EndPosition);
        NewValue->Context = BaseHandle->Context;
		NewValue->Value = BaseHandle->Value;
        return NewValue;
    }

    /**
     * Copies the value.
     * @return
     */
    [[nodiscard]]
    virtual SharedPtr<ValueHandle> Copy() const = 0;

    /**
     * Executes the handle.
     * @param ArgValueList
     * @return
     */
    virtual SharedPtr<class ParserRuntimeResult> Exec(const std::vector<SharedPtr<ValueHandle>>& ArgValueList);

	/**
	 * Retrieves an illegal operation exception.
	 * @param pValue
	 * @return
	 */
	static SharedPtr<ValueHandle> GetIllegalOperationException(SharedPtr<ValueHandle> pValue);

	/**
	 * Retrieve the value handle for null.
	 * @return
	 */
	static SharedPtr<ValueHandle> Null(class ParserContext* Context = nullptr);

	/**
	 * Retrieve the value handle for true.
	 * @return
	 */
	static SharedPtr<ValueHandle> True(class ParserContext* Context = nullptr);

	/**
	 * Retrieve the value handle for false.
	 * @return
	 */
	static SharedPtr<ValueHandle> False(class ParserContext* Context = nullptr);

    /**
     * Returns a string of the value.
     * @return
     */
    virtual CStrPtr ToString();
};

SymplNamespaceEnd
