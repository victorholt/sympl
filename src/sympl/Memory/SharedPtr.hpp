//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include <sympl/System/SymplPCH.hpp>
#include "SharedPtrRef.hpp"

SymplNamespaceStart

template<typename T>
class SharedPtr
{
private:
	// Pointer reference.
	T*    PtrData;

	// Reference counter.
	SharedPtrRef* RefCounter;

public:
	/**
	 * Constructor.
	 */
	SharedPtr();

	/**
	 * Constructor.
	 * @param Value
	 */
	SharedPtr(T* Value);

	/**
	 * Constructor.
	 * @param CopySharedPtr
	 */
	SharedPtr(const SharedPtr<T>& CopySharedPtr);

	/**
	 * Destructor.
	 */
	~SharedPtr();

	/**
	 * Overloads the pointer operator.
	 * @return
	 */
	T& operator* ();

	/**
	 * Overloads the pointer access operator.
	 * @return
	 */
	T* operator-> ();

	/**
	 * Overloads the = operator.
	 * @param Ptr
	 * @return
	 */
	SharedPtr<T>& operator = (const SharedPtr<T>& Ptr);

	/**
	 * Returns the pointer data.
	 * @return
	 */
	inline T* Ptr() const { return PtrData; }

	/**
	 * Returns the current count.
	 * @return
	 */
	inline size_t RefCount() const { return RefCounter->Count; }

	/**
	 * Returns whether or not the data is valid.
	 * @return
	 */
	inline bool IsValid() const { return PtrData != nullptr && RefCounter->Count > 0; }
};

template<typename T>
SharedPtr<T>::SharedPtr()
		: PtrData(nullptr), RefCounter(nullptr)
{
	RefCounter = new SharedPtrRef();
	RefCounter->AddRef();
}

template<typename T>
SharedPtr<T>::SharedPtr(T* Value)
		: PtrData(Value), RefCounter(nullptr)
{
	RefCounter = new SharedPtrRef();
	RefCounter->AddRef();
}

template<typename T>
SharedPtr<T>::SharedPtr(const SharedPtr<T> &CopySharedPtr)
		: PtrData(CopySharedPtr.PtrData), RefCounter(nullptr)
{
	RefCounter = CopySharedPtr.RefCounter;
	RefCounter->AddRef();
}

template<typename T>
SharedPtr<T>::~SharedPtr()
{
	if (RefCounter->Release() == 0)
	{
		delete PtrData;
		delete RefCounter;
	}
}

template<typename T>
T& SharedPtr<T>::operator*()
{
	return *PtrData;
}

template<typename T>
T* SharedPtr<T>::operator->()
{
	return PtrData;
}

template<typename T>
SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr<T> &Ptr)
{
	if (this == &Ptr)
	{
		return *this;
	}

	// Check if we need to delete the data.
	if(RefCounter->Release() == 0)
	{
		delete PtrData;
		delete RefCounter;
	}

	// Copy over our data.
	PtrData = Ptr.PtrData;
	RefCounter = Ptr.RefCounter;
	RefCounter->AddRef();

	return *this;
}

SymplNamespaceEnd