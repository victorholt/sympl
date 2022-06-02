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

protected:

    // String representation of the number.
    char TmpNumber_Allocation[64];

    // Start position of the number value.
    SharedPtr<struct LexerPosition> StartPosition;
    // End position of the number value.
    SharedPtr<struct LexerPosition> EndPosition;

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
    void SetPosition(SharedPtr<class LexerPosition> pStartPosition, SharedPtr<class LexerPosition> pEndPosition);

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

    [[nodiscard]]
    virtual SharedPtr<ValueHandle> Copy() const;

    /**
     * Executes the handle.
     * @param ArgValueList
     * @return
     */
    virtual SharedPtr<class ParserRuntimeResult> Exec(const std::vector<SharedPtr<ValueHandle>>& ArgValueList);

    /**
     * Returns a string of the value.
     * @return
     */
    virtual CStrPtr ToString();
};

SymplNamespaceEnd
