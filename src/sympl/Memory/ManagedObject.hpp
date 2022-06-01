//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include "MemBlock.hpp"
#include "MemPool.hpp"
#include "SharedPtr.hpp"
#include "ObjectRef.hpp"

SymplNamespaceStart

class SYMPL_API ManagedObject : public ObjectRef
{
private:
	// Reference to the memory block.
	MemBlock* Block;
    // Size of the object.
    size_t ObjectSize = 0;

public:
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
     * @param ArgList
     */
    virtual void __Construct(int argc, va_list ArgList);

    /**
	 * Subtract from the reference count.
	 */
    virtual int Release() override;

    /**
     * Performs a memory-based copy.
     * @return
     */
    ManagedObject* MemCopy();

	/**
	 * Creates a new managed object.
	 * @tparam T
	 * @return
	 */
	template<class T>
	static SharedPtr<T> New()
	{
		size_t ObjectSize = sizeof(T);
		MemBlock* Block = MemPool::Instance()->CreateBlock(ObjectSize);

//		ManagedObject* NewObject = reinterpret_cast<T*>(Block->Bytes);
		ManagedObject* NewObject = new(Block->Bytes) T();
		NewObject->Block = Block;
        NewObject->ObjectSize = ObjectSize;

		return SharedPtr<T>(static_cast<T*>(NewObject));
	}

    /**
     * Creates a new managed object.
     * @tparam T
     * @tparam R
     * @return
     */
    template<class T, class R>
    static SharedPtr<R> New()
    {
        size_t ObjectSize = sizeof(T);
        MemBlock* Block = MemPool::Instance()->CreateBlock(ObjectSize);

        ManagedObject* NewObject = new(Block->Bytes) T();
        NewObject->Block = Block;
        NewObject->ObjectSize = ObjectSize;

        return SharedPtr<R>(static_cast<R*>(NewObject));
    }

    /**
     * Allocates memory without using the memory pool.
     * @tparam T
     * @return
     */
    template<class T>
    static SharedPtr<T> Alloc(int argc = 0, ...)
    {
        char* Bytes = static_cast<char*>(malloc(sizeof(T)));
        ManagedObject* NewObject = new(Bytes) T();
        NewObject->ObjectSize = sizeof(T);

        if (argc > 0) {
            va_list ArgList;
            va_start(ArgList, argc);
            NewObject->__Construct(argc, ArgList);
            va_end(ArgList);
        }

        return SharedPtr<T>(static_cast<T*>(NewObject));
    }

    /**
     * Allocates memory without using the memory pool.
     * @tparam T
     * @tparam R
     * @return
     */
    template<class T, class R>
    static SharedPtr<R> Alloc(int argc = 0, ...)
    {
        char* Bytes = static_cast<char*>(malloc(sizeof(T)));
        ManagedObject* NewObject = new(Bytes) T();
        NewObject->ObjectSize = sizeof(T);

        if (argc > 0) {
            va_list ArgList;
            va_start(ArgList, argc);
            NewObject->__Construct(argc, ArgList);
            va_end(ArgList);
        }

        return SharedPtr<R>(dynamic_cast<R*>(NewObject));
    }
};

SymplNamespaceEnd
