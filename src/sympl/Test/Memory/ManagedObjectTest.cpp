//
// GameSencha, LLC 5/25/22.
//
#include "ManagedObjectTest.hpp"
SymplNamespace

void ManagedObjectTest::Run()
{
    auto mC = new ManagedClass();
	SharedPtr<ManagedClass> managedClass = SharedPtr<ManagedClass>(mC); //ManagedObject::New<ManagedClass>();

	{
		SharedPtr<ManagedClass> managedClass1 = managedClass;
		std::cout << "ManagedClass RefCount: " << managedClass.RefCount() << std::endl;

//        managedClass = new ManagedClass();
	}

	std::cout << "ManagedClass RefCount: " << managedClass.RefCount() << std::endl;
	std::cout << "ManagedClass Buffer: " << managedClass->buffer << std::endl;
}
