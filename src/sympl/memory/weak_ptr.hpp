//
// GameSencha, LLC 1/31/24.
//
#pragma once
#include "shared_ptr.hpp"

SymplNamespaceStart

template<typename T>
class WeakPtr
{
private:
    // Pointer reference.
    ObjectRef* ptr_data = nullptr;

public:
    /**
     * Constructor.
     */
    WeakPtr();

    /**
     * Constructor.
     * @param p_value
     */
    WeakPtr(T* p_value);

    /**
     * Constructor.
     * @param p_copy_weak_ptr
     */
    WeakPtr(const WeakPtr<T>& p_copy_weak_ptr);

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
     * @param p_ptr
     * @return
     */
    WeakPtr<T>& operator = (const WeakPtr<T>& p_ptr);
    WeakPtr<T>& operator = (const SharedPtr<T>& p_ptr);
    WeakPtr<T>& operator = (T* Ptr);

    /**
     * Returns the pointer data.
     * @return
     */
    inline T* ptr() const { return static_cast<T*>(ptr_data); }

    /**
     * Returns the current count.
     * @return
     */
    inline size_t ref_count() const { return ptr_data->ref_count; }

    /**
     * Returns whether or not the data is valid.
     * @return
     */
    inline bool is_value() const { return ptr_data != nullptr && ptr_data->ref_count > 0; }
};

template<typename T>
WeakPtr<T>::WeakPtr()
{
    ptr_data = nullptr;
}

template<typename T>
WeakPtr<T>::WeakPtr(T* p_value)
{
    ptr_data = p_value;
}

template<typename T>
WeakPtr<T>::WeakPtr(const WeakPtr<T> &p_copy_weak_ptr)
{
    ptr_data = p_copy_weak_ptr.ptr_data;
}

template<typename T>
WeakPtr<T>::~WeakPtr()
{
    if (ptr_data && ptr_data->ref_count <= 0) {
        return;
    }

    ptr_data = nullptr;
}

template<typename T>
T& WeakPtr<T>::operator*()
{
    return *ptr_data;
}

template<typename T>
const T& WeakPtr<T>::operator*() const
{
    return *ptr_data;
}

template<typename T>
T* WeakPtr<T>::operator->()
{
    return static_cast<T*>(ptr_data);
}

template<typename T>
const T* WeakPtr<T>::operator->() const
{
    return static_cast<T*>(ptr_data);
}

template<typename T>
WeakPtr<T> &WeakPtr<T>::operator=(const WeakPtr<T> &p_ptr)
{
    if (this == &p_ptr)
    {
        return *this;
    }

    if (ptr_data && ptr_data == p_ptr.ptr_data) {
        return *this;
    }

    // Copy over our data.
    ptr_data = p_ptr.ptr_data;

    return *this;
}

template<typename T>
WeakPtr<T>& WeakPtr<T>::operator=(const SharedPtr<T>& p_ptr)
{
    ptr_data = p_ptr.ptr_data;
    return *this;
}

template<typename T>
WeakPtr<T> &WeakPtr<T>::operator=(T* p_ptr)
{
    // Check if we need to delete the data.
    if (ptr_data && ptr_data == p_ptr) {
        return *this;
    }

    // Copy over our data.
    ptr_data = p_ptr;

    return *this;
}

SymplNamespaceEnd