//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "ObjectRefInfo.hpp"
#include <sympl/include/Memory/MemBlock.hpp>

SymplNamespaceStart

class SYMPL_API ObjectRef
{
public:
	// Reference count.
	long long RefCount = 0;

    // Reference to the memory block.
    MemBlock* Block = nullptr;

    /**
     * Destructor.
     */
    virtual ~ObjectRef() {}

	/**
	 * Adds to the reference count.
	 */
	void AddRef()
	{
		if (Block && Block->Static) {
			RefCount = 1;
		}

		// Increment the reference count
        RefCount++;
	}

	/**
	 * Subtracts the reference count.
	 * @return
	 */
	virtual int Release()
	{
		if (Block && Block->Static) {
			return 1;
		}

		// Decrement the reference count and
		// return the reference count.
		return --RefCount;
	}

	/**
	 * Overloads the equal operator.
	 * @param rhs
	 * @return
	 */
	ObjectRef& operator=(const ObjectRef& rhs)
	{
		if (this == &rhs) {
			return *this;
		}

        RefCount = rhs.RefCount;

		return *this;
	}

	//! Return type name.
	//! \return string
	virtual const std::string& GetTypeName() const = 0;

	//! Return type info.
	//! \return RefInfo
	virtual const ObjectRefInfo* GetTypeInfo() const = 0;

	//! Return type info static.
	//! \return RefInfo
	static const ObjectRefInfo* GetTypeInfoStatic() { return nullptr; }

    //! Casts to another value.
    //! \tparam T
    //! \param CastValue
    //! \return
    template<class T>
    static T* CastTo(ObjectRef* CastValue) {
        return dynamic_cast<T*>(CastValue);
    }
};

SymplNamespaceEnd
