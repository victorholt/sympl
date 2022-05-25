//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include <sympl/System/SymplPCH.hpp>

SymplNamespaceStart

class SharedPtrRef
{
public:
	// Reference count.
	size_t Count = 0;

	/**
	 * Adds to the reference count.
	 */
	void AddRef()
	{
		// Increment the reference count
		Count++;
	}

	/**
	 * Subtracts the reference count.
	 * @return
	 */
	int Release()
	{
		// Decrement the reference count and
		// return the reference count.
		return --Count;
	}

	/**
	 * Overloads the equal operator.
	 * @param rhs
	 * @return
	 */
	SharedPtrRef& operator=(const SharedPtrRef& rhs)
	{
		if (this == &rhs) {
			return *this;
		}

		Count = rhs.Count;

		return *this;
	}
};

SymplNamespaceEnd