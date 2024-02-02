//
// GameSencha, LLC 1/31/24.
//
#pragma once
#include "mem_pool.hpp"
#include "object_ref.hpp"

SymplNamespaceStart

template<typename T>
class SharedPtr
{
    template<class R>
    friend class WeakPtr;

private:
	// Pointer reference.
	ObjectRef* ptr_data = nullptr;

public:
	/**
	 * Constructor.
	 */
	SharedPtr();

	/**
	 * Constructor.
	 * @param p_value
	 */
	SharedPtr(T* p_value);

	/**
	 * Constructor.
	 * @param p_copy_shared_ptr
	 */
	SharedPtr(const SharedPtr<T>& p_copy_shared_ptr);

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
	 * @param p_ptr
	 * @return
	 */
	SharedPtr<T>& operator = (const SharedPtr<T>& p_ptr);
    SharedPtr<T>& operator = (T* p_ptr);

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
	inline bool is_valid() const { return ptr_data != nullptr && ptr_data->ref_count > 0; }
};

template<typename T>
SharedPtr<T>::SharedPtr()
{
    ptr_data = nullptr;
}

template<typename T>
SharedPtr<T>::SharedPtr(T* p_value)
{
    ptr_data = p_value;
    if (ptr_data) {
        ptr_data->add_ref();
    }
}

template<typename T>
SharedPtr<T>::SharedPtr(const SharedPtr<T> &p_copy_shared_ptr)
{
    if (ptr_data) {
        ptr_data->release();
    }

    ptr_data = p_copy_shared_ptr.ptr_data;
    if (ptr_data) {
        ptr_data->add_ref();
    }
}

template<typename T>
SharedPtr<T>::~SharedPtr()
{
    if (ptr_data && ptr_data->ref_count <= 0) {
        return;
    }

	if (ptr_data && ptr_data->release() == 0)
	{
		MemPool::instance()->free_block(ptr_data->mem_block);
        ptr_data = nullptr;
	}
}

template<typename T>
T& SharedPtr<T>::operator*()
{
	return *ptr_data;
}

template<typename T>
const T& SharedPtr<T>::operator*() const
{
    return *ptr_data;
}

template<typename T>
T* SharedPtr<T>::operator->()
{
	return static_cast<T*>(ptr_data);
}

template<typename T>
const T* SharedPtr<T>::operator->() const
{
    return static_cast<T*>(ptr_data);
}

template<typename T>
SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr<T> &p_ptr)
{
	if (this == &p_ptr)
	{
		return *this;
	}

    if (ptr_data && ptr_data == p_ptr.ptr_data) {
        ptr_data->add_ref();
        return *this;
    }

	// Check if we need to delete the data.
	if(ptr_data && ptr_data->mem_block && !ptr_data->mem_block->is_static && ptr_data->release() == 0)
	{
		MemPool::instance()->free_block(ptr_data->mem_block);
	}

	// Copy over our data.
	ptr_data = p_ptr.ptr_data;

    if (ptr_data) {
        ptr_data->add_ref();
    }

	return *this;
}

template<typename T>
SharedPtr<T> &SharedPtr<T>::operator=(T* p_ptr)
{
    // Check if we need to delete the data.
    if(ptr_data && ptr_data != p_ptr && ptr_data->release() == 0)
    {
        MemPool::instance()->free_block(ptr_data->mem_block);
        ptr_data = nullptr;
    }

    if (ptr_data && ptr_data == p_ptr) {
        ptr_data->add_ref();
        return *this;
    }

    // Copy over our data.
    ptr_data = p_ptr;
    if (ptr_data) {
        ptr_data->add_ref();
    }

    return *this;
}

SymplNamespaceEnd