//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include <__sympl_old/include/Memory/MemPool.hpp>
#include "ObjectRef.hpp"

SymplNamespaceStart

template<typename T>
class SharedPtr
{
    template<class R>
    friend class WeakPtr;

private:
	// Pointer reference.
	ObjectRef* PtrData = nullptr;

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
	const T& operator* () const;

	/**
	 * Overloads the pointer access operator.
	 * @return
	 */
	T* operator-> ();
    const T* operator->() const;

	/**
	 * Overloads the = operator.
	 * @param Ptr
	 * @return
	 */
	SharedPtr<T>& operator = (const SharedPtr<T>& Ptr);
    SharedPtr<T>& operator = (T* Ptr);

	/**
	 * Returns the pointer data.
	 * @return
	 */
	inline T* Ptr() const { return static_cast<T*>(PtrData); }

	/**
	 * Returns the current count.
	 * @return
	 */
	inline size_t RefCount() const { return PtrData->RefCount; }

	/**
	 * Returns whether or not the data is valid.
	 * @return
	 */
	inline bool IsValid() const { return PtrData != nullptr && PtrData->RefCount > 0; }
};

template<typename T>
SharedPtr<T>::SharedPtr()
{
    PtrData = nullptr;
}

template<typename T>
SharedPtr<T>::SharedPtr(T* Value)
{
    PtrData = Value;
    if (PtrData) {
        PtrData->AddRef();
    }
}

template<typename T>
SharedPtr<T>::SharedPtr(const SharedPtr<T> &CopySharedPtr)
{
    if (PtrData) {
        PtrData->Release();
    }

    PtrData = CopySharedPtr.PtrData;
    if (PtrData) {
        PtrData->AddRef();
    }
}

template<typename T>
SharedPtr<T>::~SharedPtr()
{
    if (PtrData && PtrData->RefCount <= 0) {
        return;
    }

	if (PtrData && PtrData->Release() == 0)
	{
		MemPool::Instance()->FreeBlock(PtrData->Block);
        PtrData = nullptr;
	}
}

template<typename T>
T& SharedPtr<T>::operator*()
{
	return *PtrData;
}

template<typename T>
const T& SharedPtr<T>::operator*() const
{
    return *PtrData;
}

template<typename T>
T* SharedPtr<T>::operator->()
{
	return static_cast<T*>(PtrData);
}

template<typename T>
const T* SharedPtr<T>::operator->() const
{
    return static_cast<T*>(PtrData);
}

template<typename T>
SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr<T> &Ptr)
{
	if (this == &Ptr)
	{
		return *this;
	}

    if (PtrData && PtrData == Ptr.PtrData) {
        PtrData->AddRef();
        return *this;
    }

	// Check if we need to delete the data.
	if(PtrData && PtrData->Block && !PtrData->Block->Static && PtrData->Release() == 0)
	{
		MemPool::Instance()->FreeBlock(PtrData->Block);
	}

	// Copy over our data.
	PtrData = Ptr.PtrData;

    if (PtrData) {
        PtrData->AddRef();
    }

	return *this;
}

template<typename T>
SharedPtr<T> &SharedPtr<T>::operator=(T* Ptr)
{
    // Check if we need to delete the data.
    if(PtrData && PtrData != Ptr && PtrData->Release() == 0)
    {
        MemPool::Instance()->FreeBlock(PtrData->Block);
        PtrData = nullptr;
    }

    if (PtrData && PtrData == Ptr) {
        PtrData->AddRef();
        return *this;
    }

    // Copy over our data.
    PtrData = Ptr;
    if (PtrData) {
        PtrData->AddRef();
    }

    return *this;
}

SymplNamespaceEnd
