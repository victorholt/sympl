//
// GameSencha, LLC 6/7/2022.
// Copyright (c) 2022
// All rights reserved.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/include/Memory/MemPool.hpp>
#include "ObjectRef.hpp"
#include "SharedPtr.hpp"

SymplNamespaceStart

template<typename T>
class WeakPtr
{
private:
    // Pointer reference.
    ObjectRef* PtrData = nullptr;

public:
    /**
     * Constructor.
     */
    WeakPtr();

    /**
     * Constructor.
     * @param Value
     */
    WeakPtr(T* Value);

    /**
     * Constructor.
     * @param CopyWeakPtr
     */
    WeakPtr(const WeakPtr<T>& CopyWeakPtr);

    /**
     * Destructor.
     */
    ~WeakPtr();

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
    WeakPtr<T>& operator = (const WeakPtr<T>& Ptr);
    WeakPtr<T>& operator = (const SharedPtr<T>& Ptr);
    WeakPtr<T>& operator = (T* Ptr);

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
WeakPtr<T>::WeakPtr()
{
    PtrData = nullptr;
}

template<typename T>
WeakPtr<T>::WeakPtr(T* Value)
{
    PtrData = Value;
}

template<typename T>
WeakPtr<T>::WeakPtr(const WeakPtr<T> &CopyWeakPtr)
{
    PtrData = CopyWeakPtr.PtrData;
}

template<typename T>
WeakPtr<T>::~WeakPtr()
{
    if (PtrData && PtrData->RefCount <= 0) {
        return;
    }

    PtrData = nullptr;
}

template<typename T>
T& WeakPtr<T>::operator*()
{
    return *PtrData;
}

template<typename T>
const T& WeakPtr<T>::operator*() const
{
    return *PtrData;
}

template<typename T>
T* WeakPtr<T>::operator->()
{
    return static_cast<T*>(PtrData);
}

template<typename T>
const T* WeakPtr<T>::operator->() const
{
    return static_cast<T*>(PtrData);
}

template<typename T>
WeakPtr<T> &WeakPtr<T>::operator=(const WeakPtr<T> &Ptr)
{
    if (this == &Ptr)
    {
        return *this;
    }

    if (PtrData && PtrData == Ptr.PtrData) {
        return *this;
    }

    // Copy over our data.
    PtrData = Ptr.PtrData;

    return *this;
}

template<typename T>
WeakPtr<T>& WeakPtr<T>::operator=(const SharedPtr<T>& Ptr)
{
    PtrData = Ptr.PtrData;
    return *this;
}

template<typename T>
WeakPtr<T> &WeakPtr<T>::operator=(T* Ptr)
{
    // Check if we need to delete the data.
    if (PtrData && PtrData == Ptr) {
        return *this;
    }

    // Copy over our data.
    PtrData = Ptr;

    return *this;
}

SymplNamespaceEnd
