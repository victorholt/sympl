//
// GameSencha, LLC 1/31/24.
//
#pragma once
#include "mem_block.hpp"
#include "mem_pool.hpp"
#include "shared_ptr.hpp"
#include "object_ref.hpp"

SymplNamespaceStart

class SYMPL_API ManagedObject : public ObjectRef
{
private:
    // Size of the object.
    size_t object_size = 0;

public:
    // The instance id.
    long long instance_id = -1;

	/**
	 * Constructor.
	 */
	ManagedObject();

	/**
	 * Destructor.
	 */
	~ManagedObject();

    /**
     * Constructs the object.
     * @param p_arg_list
     */
    virtual void __construct(int argc, va_list p_arg_list);

    /**
     * Destructor.
     */
    virtual void __destruct() {}

    /**
	 * Subtract from the reference count.
	 */
    virtual int release() override;

    /**
     * Performs a memory-based copy.
     * @return
     */
    ManagedObject* mem_copy();

	/**
	 * Creates a new managed object.
	 * @tparam T
	 * @return
	 */
	template<class T>
	static SharedPtr<T> __new()
	{
		size_t object_size = sizeof(T);
		MemBlock* mem_block = MemPool::instance()->create_block(object_size);
		assert(mem_block != nullptr);

		ManagedObject* new_object = new(mem_block->bytes) T();
		new_object->mem_block = mem_block;
        new_object->object_size = object_size;
        new_object->instance_id = _sympl_object_next_instance_id++;
		mem_block->set_identifier(new_object->get_type_name().c_str());

		return SharedPtr<T>(static_cast<T*>(new_object));
	}

    /**
     * Creates a new managed object.
     * @tparam T
     * @tparam R
     * @return
     */
    template<class T, class R>
    static SharedPtr<R> __new()
    {
        size_t object_size = sizeof(T);
        MemBlock* mem_block = MemPool::instance()->create_block(object_size);
		assert(mem_block != nullptr);

        ManagedObject* new_object = new(mem_block->bytes) T();
        new_object->mem_block = mem_block;
        new_object->object_size = object_size;
        new_object->instance_id = _sympl_object_next_instance_id++;
		mem_block->set_identifier(new_object->get_type_name().c_str());

        return SharedPtr<R>(static_cast<R*>(new_object));
    }

    /**
     * Allocates memory without using the memory pool.
     * @tparam T
     * @return
     */
    template<class T>
    static SharedPtr<T> alloc(int argc = 0, ...)
    {
        size_t l_object_size = sizeof(T);

        char* bytes = static_cast<char*>(malloc(sizeof(T)));
        ManagedObject* new_object = new(bytes) T();
        new_object->object_size = l_object_size;
        new_object->instance_id = _sympl_object_next_instance_id++;

        if (argc > 0) {
            va_list arg_list;
            va_start(arg_list, argc);
            new_object->__construct(argc, arg_list);
            va_end(arg_list);
        } else {
            new_object->__construct(0, nullptr);
        }

        return SharedPtr<T>(static_cast<T*>(new_object));
    }

    /**
     * Allocates memory without using the memory pool.
     * @tparam T
     * @tparam R
     * @return
     */
    template<class T, class R>
    static SharedPtr<R> alloc(int argc = 0, ...)
    {
        char* Bytes = static_cast<char*>(malloc(sizeof(T)));
        ManagedObject* new_object = new(Bytes) T();
        new_object->object_size = sizeof(T);
        new_object->instance_id = _sympl_object_next_instance_id++;

        if (argc > 0) {
            va_list arg_list;
            va_start(arg_list, argc);
            new_object->__construct(argc, arg_list);
            va_end(arg_list);
        } else {
            new_object->__construct(0, nullptr);
        }

        return SharedPtr<R>(dynamic_cast<R*>(new_object));
    }
};

SymplNamespaceEnd