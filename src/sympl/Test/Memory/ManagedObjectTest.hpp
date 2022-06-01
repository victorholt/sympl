//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include <sympl/Sympl.hpp>

SymplNamespaceStart

class ManagedClass : public ManagedObject
{
	SYMPL_OBJECT(ManagedClass, ManagedObject)

public:
	char buffer[256];

	ManagedClass()
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "Managed Class String");
	}
};

class ManagedObjectTest
{
public:

	void Run();
};

SymplNamespaceEnd
