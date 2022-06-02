//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "ObjectRefInfo.hpp"

SymplNamespaceStart

class SYMPL_API ObjectRef
{
public:
	// Reference count.
	size_t RefCount = 0;

    /**
     * Destructor.
     */
    virtual ~ObjectRef() {}

	/**
	 * Adds to the reference count.
	 */
	void AddRef()
	{
		// Increment the reference count
        RefCount++;
	}

	/**
	 * Subtracts the reference count.
	 * @return
	 */
	virtual int Release()
	{
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
};

SymplNamespaceEnd
