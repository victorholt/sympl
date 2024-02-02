//
// GameSencha, LLC 1/31/24.
//
#pragma once
#include "mem_block.hpp"
#include "object_ref_info.hpp"

SymplNamespaceStart

class SYMPL_API ObjectRef
{
public:
	// Reference count.
	long long ref_count = 0;

	// Reference to the memory block.
	MemBlock* mem_block = nullptr;

	/**
	 * Destructor.
	 */
	virtual ~ObjectRef() {}

	/**
	 * Adds to the reference count.
	 */
	void add_ref()
	{
		if (mem_block && mem_block->is_static) {
			ref_count = 1;
		}

		// Increment the reference count
		ref_count++;
	}

	/**
	 * Subtracts the reference count.
	 * @return
	 */
	virtual int release()
	{
		if (mem_block && mem_block->is_static) {
			return 1;
		}

		// Decrement the reference count and
		// return the reference count.
		return --ref_count;
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

		ref_count = rhs.ref_count;

		return *this;
	}

	//! Return type name.
	//! \return string
	virtual const std::string& get_type_name() const = 0;

	//! Return type info.
	//! \return RefInfo
	virtual const ObjectRefInfo* get_type_info() const = 0;

	//! Return type info static.
	//! \return RefInfo
	static const ObjectRefInfo* get_type_info_static() { return nullptr; }

	//! Casts to another value.
	//! \tparam T
	//! \param p_cast_value
	//! \return
	template<class T>
	static T* cast_to(ObjectRef* p_cast_value) {
		return dynamic_cast<T*>(p_cast_value);
	}
};

SymplNamespaceEnd