//
// GameSencha, LLC 5/25/22.
//
#include "ManagedObjectTest.hpp"
SymplNamespace

void ManagedObjectTest::Run()
{
	SharedPtr<ManagedClass> managedClass = ManagedObject::New<ManagedClass>();

	{
		SharedPtr<ManagedClass> managedClass1 = managedClass;
		std::cout << "ManagedClass RefCount: " << managedClass.RefCount() << std::endl;
	}

	std::cout << "ManagedClass RefCount: " << managedClass.RefCount() << std::endl;
	std::cout << "ManagedClass Buffer: " << managedClass->buffer << std::endl;
}
