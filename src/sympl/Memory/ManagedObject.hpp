//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include <sympl/System/SymplPCH.hpp>
#include "MemBlock.hpp"
#include "MemPool.hpp"
#include "SharedPtr.hpp"

SymplNamespaceStart

class ManagedObject
{
private:
	// Reference to the memory block.
	MemBlock* Block;

protected:

	/**
	 * Subtract from the reference count.
	 */
	void Release();

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
	 * Makes a new managed object.
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

		return SharedPtr<T>(static_cast<T*>(NewObject));
	}

};

SymplNamespaceEnd